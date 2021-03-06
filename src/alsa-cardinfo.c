#include <node_api.h>
#include <stdio.h>
#include <alsa/asoundlib.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof *(a))

static const snd_pcm_access_t accesses[] = {
    SND_PCM_ACCESS_MMAP_INTERLEAVED,
    SND_PCM_ACCESS_MMAP_NONINTERLEAVED,
    SND_PCM_ACCESS_MMAP_COMPLEX,
    SND_PCM_ACCESS_RW_INTERLEAVED,
    SND_PCM_ACCESS_RW_NONINTERLEAVED,
};

static const snd_pcm_format_t formats[] = {

    SND_PCM_FORMAT_S8,
    SND_PCM_FORMAT_U8,
    SND_PCM_FORMAT_S16_LE,
    SND_PCM_FORMAT_S16_BE,
    SND_PCM_FORMAT_U16_LE,
    SND_PCM_FORMAT_U16_BE,
    SND_PCM_FORMAT_S24_LE,
    SND_PCM_FORMAT_S24_BE,
    SND_PCM_FORMAT_U24_LE,
    SND_PCM_FORMAT_U24_BE,
    SND_PCM_FORMAT_S32_LE,
    SND_PCM_FORMAT_S32_BE,
    SND_PCM_FORMAT_U32_LE,
    SND_PCM_FORMAT_U32_BE,
    SND_PCM_FORMAT_FLOAT_LE,
    SND_PCM_FORMAT_FLOAT_BE,
    SND_PCM_FORMAT_FLOAT64_LE,
    SND_PCM_FORMAT_FLOAT64_BE,
    SND_PCM_FORMAT_IEC958_SUBFRAME_LE,
    SND_PCM_FORMAT_IEC958_SUBFRAME_BE,
    SND_PCM_FORMAT_MU_LAW,
    SND_PCM_FORMAT_A_LAW,
    SND_PCM_FORMAT_IMA_ADPCM,
    SND_PCM_FORMAT_MPEG,
    SND_PCM_FORMAT_GSM,
    SND_PCM_FORMAT_SPECIAL,
    SND_PCM_FORMAT_S24_3LE,
    SND_PCM_FORMAT_S24_3BE,
    SND_PCM_FORMAT_U24_3LE,
    SND_PCM_FORMAT_U24_3BE,
    SND_PCM_FORMAT_S20_3LE,
    SND_PCM_FORMAT_S20_3BE,
    SND_PCM_FORMAT_U20_3LE,
    SND_PCM_FORMAT_U20_3BE,
    SND_PCM_FORMAT_S18_3LE,
    SND_PCM_FORMAT_S18_3BE,
    SND_PCM_FORMAT_U18_3LE,
    SND_PCM_FORMAT_U18_3BE,
    SND_PCM_FORMAT_G723_24,
    SND_PCM_FORMAT_G723_24_1B,
    SND_PCM_FORMAT_G723_40,
    SND_PCM_FORMAT_G723_40_1B,
    SND_PCM_FORMAT_DSD_U8,
    SND_PCM_FORMAT_DSD_U16_LE,
    //SND_PCM_FORMAT_DSD_U32_LE,
    //SND_PCM_FORMAT_DSD_U16_BE,
    //SND_PCM_FORMAT_DSD_U32_BE,
};

static const unsigned int rates[] = {
    5512,
    8000,
    11025,
    16000,
    22050,
    32000,
    44100,
    48000,
    64000,
    88200,
    96000,
    176400,
    192000,
};

napi_value GetCardInfo(napi_env env, napi_callback_info info)
{
    size_t result = 0;
    size_t argc = 2;
    snd_pcm_t *pcm;
    snd_pcm_hw_params_t *hw_params;
    uint32_t stream_type = 0;
    uint32_t arg1 = 0;
    char *arg0;
    const char *device_name = "hw";
    unsigned int i;
    unsigned int min, max;
    int err;
    char error[80] = {"\0"};
    char extError[200] = {"\0"};

    napi_value argv[2];
    napi_status status;
    napi_value errorText;
    napi_value extErrorText;
    napi_value deviceType;
    napi_value accessTypesArray;
    napi_value formatArray;
    napi_value channelsArray;
    napi_value sampleRatesArray;
    //napi_value periodTime;
    //napi_value bufferTime;

    napi_value returnObj;
    status = napi_create_object(env, &returnObj);
    if (status != napi_ok)
        napi_throw_error(env, NULL, "Unable to create return object");

    status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

    if (status != napi_ok)
        napi_throw_error(env, NULL, "Failed to parse arguments");

    status = napi_get_value_string_utf8(env, argv[0], NULL, 0, &result);
    if (status != napi_ok)
        napi_throw_error(env, NULL, "Failed to get argument 0");

    arg0 = (char*)malloc(result + 1);
    status = napi_get_value_string_utf8(env, argv[0], arg0, result + 1, &result);

    if (status == napi_ok)
    {
        device_name = arg0;
        status = napi_get_value_uint32(env, argv[1], &arg1);

        if (status == napi_ok)
        {
            stream_type = arg1;
        }
    }

    void alsaErrorHandler(const char *filename, int line, const char *function, int error, const char *fmt, ...)
    {
        strcpy(extError, filename);
        strcat(extError, ":");
        char buffer[60];
        sprintf(buffer, "%d", line);
        strcat(extError, buffer);
        strcat(extError, "(");
        strcat(extError, function);
        strcat(extError, ") ");
        sprintf(buffer, fmt, device_name, error);
        strcat(extError, buffer);
        strcat(extError, "\0");
    };

    snd_lib_error_set_handler(&alsaErrorHandler);

    void handle_error()
    {
        napi_create_string_utf8(env, error, NAPI_AUTO_LENGTH, &errorText);
        napi_set_named_property(env, returnObj, "error", errorText);
        //strcpy(error, "\0");
        if (strlen(extError) > 0)
        {
            napi_create_string_utf8(env, extError, NAPI_AUTO_LENGTH, &extErrorText);
            napi_set_named_property(env, returnObj, "errorDetails", extErrorText);
            //strcpy(extError, "\0");
        }
    }

    err = snd_pcm_open(&pcm, device_name, stream_type, SND_PCM_NONBLOCK);
    if (err < 0)
    {
        strcpy(error, "cannot open device: ");
        strcat(error, device_name);
        strcat(error, " - ");
        strcat(error, snd_strerror(err));

        handle_error();
        return returnObj;
    }

    snd_pcm_hw_params_alloca(&hw_params);
    err = snd_pcm_hw_params_any(pcm, hw_params);
    if (err < 0)
    {
        strcpy(error, "cannot get device parameters: ");
        strcat(error, snd_strerror(err));
        snd_pcm_close(pcm);

        handle_error();
        return returnObj;
    }

    napi_create_string_utf8(env, snd_pcm_type_name(snd_pcm_type(pcm)), NAPI_AUTO_LENGTH, &deviceType);

    napi_create_array(env, &accessTypesArray);

    for (i = 0; i < ARRAY_SIZE(accesses); ++i)
    {
        if (!snd_pcm_hw_params_test_access(pcm, hw_params, accesses[i]))
        {
            napi_value accessType;
            napi_create_string_utf8(env, snd_pcm_access_name(accesses[i]), NAPI_AUTO_LENGTH, &accessType);

            uint32_t index;
            napi_get_array_length(env, accessTypesArray, &index);
            napi_set_element(env, accessTypesArray, index, accessType);
        }
    }

    napi_create_array(env, &formatArray);

    for (i = 0; i < ARRAY_SIZE(formats); ++i)
    {
        if (!snd_pcm_hw_params_test_format(pcm, hw_params, formats[i]))
        {
            napi_value format;
            napi_create_string_utf8(env, snd_pcm_format_name(formats[i]), NAPI_AUTO_LENGTH, &format);

            uint32_t index;
            napi_get_array_length(env, formatArray, &index);
            napi_set_element(env, formatArray, index, format);
        }
    }

    err = snd_pcm_hw_params_get_channels_min(hw_params, &min);
    if (err < 0)
    {
        strcpy(error, "cannot get minimum channels count: ");
        strcat(error, snd_strerror(err));
        snd_pcm_close(pcm);

        handle_error();
        return returnObj;
    }

    err = snd_pcm_hw_params_get_channels_max(hw_params, &max);
    if (err < 0)
    {
        strcpy(error, "cannot get maximum channels count: ");
        strcat(error, snd_strerror(err));
        snd_pcm_close(pcm);

        handle_error();
        return returnObj;
    }

    status = napi_create_array(env, &channelsArray);

    if (status != napi_ok)
        napi_throw_error(env, NULL, "Unable to create channels array");

    if (max > 50)
    {
        max = 50;
    }

    for (i = min; i <= max; ++i)
    {
        if (!snd_pcm_hw_params_test_channels(pcm, hw_params, i))
        {
            napi_value channel;
            napi_create_int32(env, i, &channel);

            uint32_t index;
            napi_get_array_length(env, channelsArray, &index);
            napi_set_element(env, channelsArray, index, channel);
        }
    }

    err = snd_pcm_hw_params_get_rate_min(hw_params, &min, NULL);
    if (err < 0)
    {
        strcpy(error, "cannot get minimum rate: ");
        strcat(error, snd_strerror(err));
        snd_pcm_close(pcm);

        handle_error();
        return returnObj;
    }

    err = snd_pcm_hw_params_get_rate_max(hw_params, &max, NULL);
    if (err < 0)
    {
        strcpy(error, "cannot get maximum rate: ");
        strcat(error, snd_strerror(err));
        snd_pcm_close(pcm);

        handle_error();
        return returnObj;
    }

    status = napi_create_array(env, &sampleRatesArray);

    for (i = 0; i < ARRAY_SIZE(rates); ++i)
    {
        if (!snd_pcm_hw_params_test_rate(pcm, hw_params, rates[i], 0))
        {
            napi_value sampleRate;
            napi_create_uint32(env, rates[i], &sampleRate);

            uint32_t index;
            napi_get_array_length(env, sampleRatesArray, &index);
            napi_set_element(env, sampleRatesArray, index, sampleRate);
        }
    }

    /*err = snd_pcm_hw_params_get_period_time_min(hw_params, &min, NULL);
    if (err < 0)
    {
        fprintf(stderr, "cannot get minimum period time: %s\n",
                snd_strerror(err));
        snd_pcm_close(pcm);
        return;
    }
    err = snd_pcm_hw_params_get_period_time_max(hw_params, &max, NULL);
    if (err < 0)
    {
        fprintf(stderr, "cannot get maximum period time: %s\n",
                snd_strerror(err));
        snd_pcm_close(pcm);
        return;
    }
    printf("Interrupt interval: %u-%u us\n", min, max);

    err = snd_pcm_hw_params_get_buffer_time_min(hw_params, &min, NULL);
    if (err < 0)
    {
        fprintf(stderr, "cannot get minimum buffer time: %s\n",
                snd_strerror(err));
        snd_pcm_close(pcm);
        return ;
    }
    err = snd_pcm_hw_params_get_buffer_time_max(hw_params, &max, NULL);
    if (err < 0)
    {
        fprintf(stderr, "cannot get maximum buffer time: %s\n",
                snd_strerror(err));
        snd_pcm_close(pcm);
        return;
    }

    printf("Buffer size: %u-%u us\n", min, max);*/
    snd_pcm_close(pcm);

    status = napi_set_named_property(env, returnObj, "deviceType", deviceType);
    if (status != napi_ok)
        napi_throw_error(env, NULL, "Unable to create named property");
    status = napi_set_named_property(env, returnObj, "accessTypes", accessTypesArray);
    if (status != napi_ok)
        napi_throw_error(env, NULL, "Unable to create named property");
    status = napi_set_named_property(env, returnObj, "sampleFormats", formatArray);
    if (status != napi_ok)
        napi_throw_error(env, NULL, "Unable to create named property");
    status = napi_set_named_property(env, returnObj, "channels", channelsArray);
    if (status != napi_ok)
        napi_throw_error(env, NULL, "Unable to create named property");
    status = napi_set_named_property(env, returnObj, "sampleRates", sampleRatesArray);
    if (status != napi_ok)
        napi_throw_error(env, NULL, "Unable to create named property");

    return returnObj;
}

napi_value Init(napi_env env, napi_value exports)
{
    napi_status status;
    napi_value fn;

    status = napi_create_function(env, NULL, 0, GetCardInfo, NULL, &fn);
    if (status != napi_ok)
    {
        napi_throw_error(env, NULL, "Unable to wrap native function");
    }

    status = napi_set_named_property(env, exports, "get_card_info", fn);
    if (status != napi_ok)
    {
        napi_throw_error(env, NULL, "Unable to populate exports");
    }

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
