#include <node_api.h>
#include <stdio.h>
#include <alsa/asoundlib.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof *(a))
#define NAPI_CHECK( napi_call, err_msg) status = napi_call; if (status != napi_ok) napi_throw_error(env, NULL, err_msg)

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
    NAPI_CHECK( napi_create_object(env, &returnObj), "Unable to create return object");
    NAPI_CHECK( napi_get_cb_info(env, info, &argc, argv, NULL, NULL), "Failed to parse arguments");
    NAPI_CHECK( napi_get_value_string_utf8(env, argv[0], NULL, 0, &result), "Failed to get argument 0");

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

    NAPI_CHECK( 
        napi_create_string_utf8(env, snd_pcm_type_name(snd_pcm_type(pcm)), NAPI_AUTO_LENGTH, &deviceType), 
        "Could not create napi string for deviceType");

    NAPI_CHECK( napi_create_array(env, &accessTypesArray), "Could not crate NAPI array accessTypesArray");

    for (i = 0; i < ARRAY_SIZE(accesses); ++i)
    {
        if (!snd_pcm_hw_params_test_access(pcm, hw_params, accesses[i]))
        {
            napi_value accessType;
            NAPI_CHECK(
                napi_create_string_utf8(env, snd_pcm_access_name(accesses[i]), NAPI_AUTO_LENGTH, &accessType),
                "Count not creat NAPI string accessType");

            uint32_t index;
            NAPI_CHECK( napi_get_array_length(env, accessTypesArray, &index), "Count not get array length of accessTypesArray");
            NAPI_CHECK( napi_set_element(env, accessTypesArray, index, accessType), "Could not add element to array accessType");
        }
    }

    NAPI_CHECK(napi_create_array(env, &formatArray), "Could not create array formatArray");

    for (i = 0; i < ARRAY_SIZE(formats); ++i)
    {
        if (!snd_pcm_hw_params_test_format(pcm, hw_params, formats[i]))
        {
            napi_value format;
            NAPI_CHECK( napi_create_string_utf8(env, snd_pcm_format_name(formats[i]), NAPI_AUTO_LENGTH, &format), "Could not create NAPI string format");

            uint32_t index;
            NAPI_CHECK( napi_get_array_length(env, formatArray, &index), "Could not get array length of formatArray");
            NAPI_CHECK( napi_set_element(env, formatArray, index, format), "Could not append element to array formatArray");
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

    NAPI_CHECK( napi_create_array(env, &channelsArray), "Unable to create channels array");

    if (max > 50)
    {
        max = 50;
    }

    for (i = min; i <= max; ++i)
    {
        if (!snd_pcm_hw_params_test_channels(pcm, hw_params, i))
        {
            napi_value channel;
            NAPI_CHECK( napi_create_int32(env, i, &channel), "Counld not create integer channel");

            uint32_t index;
            NAPI_CHECK( napi_get_array_length(env, channelsArray, &index), "Could not get length of array channelsArray");
            NAPI_CHECK( napi_set_element(env, channelsArray, index, channel), "Could not append element to channelsArray");
        }
    }

    // define GET_HARDWARE_PARAM
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

    NAPI_CHECK( napi_create_array(env, &sampleRatesArray), "Could not create array sampleRatesArray");

    for (i = 0; i < ARRAY_SIZE(rates); ++i)
    {
        if (!snd_pcm_hw_params_test_rate(pcm, hw_params, rates[i], 0))
        {
            napi_value sampleRate;
            NAPI_CHECK( napi_create_uint32(env, rates[i], &sampleRate), "Could not create NAPI integer sampleRate");

            uint32_t index;
            NAPI_CHECK( napi_get_array_length(env, sampleRatesArray, &index), "Could not get length of array sampleRatesArray");
            NAPI_CHECK( napi_set_element(env, sampleRatesArray, index, sampleRate), "Could not append element to sampleRatesArray");
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

    snd_pcm_info_t *info_t;
    err = snd_pcm_info_malloc(&info_t);
    err = snd_pcm_info(pcm, info_t);

    //todo add these as named properties
    int card = snd_pcm_info_get_card(info_t);
    int device_id = snd_pcm_info_get_device(info_t);
    int subdevice_id = snd_pcm_info_get_subdevice(info_t);

    napi_value napiCard, napiDeviceId, napiSubdeviceId;

    NAPI_CHECK( napi_create_int32(env, card, &napiCard), "Could not create integer napiCard");
    NAPI_CHECK( napi_set_named_property(env, returnObj, "cardNum", napiCard), "Could not set property cardNum");
    NAPI_CHECK( napi_create_int32(env, device_id, &napiDeviceId), "Could not create integer napiDeviceId");
    NAPI_CHECK( napi_set_named_property(env, returnObj, "devNum", napiDeviceId), "Could not set property devNum");
    NAPI_CHECK( napi_create_int32(env, subdevice_id, &napiSubdeviceId), "Could not create integer napiSebdeviceId");
    NAPI_CHECK( napi_set_named_property(env, returnObj, "subDevNum", napiSubdeviceId), "Could not set property subDevNum");

    napi_value id;
    NAPI_CHECK( napi_create_string_utf8(env,  snd_pcm_info_get_id(info_t), NAPI_AUTO_LENGTH, &id), "Cound not create string id");
    NAPI_CHECK( napi_set_named_property(env, returnObj, "id", id), "Could not set property id");

    napi_value name;
    NAPI_CHECK( napi_create_string_utf8(env, snd_pcm_info_get_name(info_t), NAPI_AUTO_LENGTH, &name), "Could not create string name");
    NAPI_CHECK( napi_set_named_property(env, returnObj, "name", name), "Cound not set property name");

    napi_value sub_name;
    NAPI_CHECK( napi_create_string_utf8(env, snd_pcm_info_get_subdevice_name(info_t), NAPI_AUTO_LENGTH, &sub_name), "Could not create string sub_name");
    NAPI_CHECK( napi_set_named_property(env, returnObj, "subName", sub_name), "Could not set property subName");

    snd_pcm_info_free(info_t);

    snd_pcm_close(pcm);

    NAPI_CHECK( napi_set_named_property(env, returnObj, "deviceType", deviceType), "Unable to create named property: deviceType");
    NAPI_CHECK( napi_set_named_property(env, returnObj, "accessTypes", accessTypesArray), "Unable to create named property: accessTypes");
    NAPI_CHECK( napi_set_named_property(env, returnObj, "sampleFormats", formatArray), "Unable to create named property: sampleFormats");
    NAPI_CHECK( napi_set_named_property(env, returnObj, "channels", channelsArray), "Unable to create named property: channels");
    NAPI_CHECK( napi_set_named_property(env, returnObj, "sampleRates", sampleRatesArray), "Unable to create named property: sampleRates");

    return returnObj;
}

napi_value EnumeratePcmDevices(napi_env env, napi_callback_info info)
{
    int err;
    char error[80] = {"\0"};
    char extError[200] = {"\0"};

    napi_status status;
    napi_value errorText;
    napi_value extErrorText;
    napi_value deviceArray;

    napi_value returnObj;
    NAPI_CHECK( napi_create_object(env, &returnObj), "Unable to create return object");

    void handle_error()
    {
        NAPI_CHECK( napi_create_string_utf8(env, error, NAPI_AUTO_LENGTH, &errorText), "Could not create string errorText");
        NAPI_CHECK( napi_set_named_property(env, returnObj, "error", errorText), "Could not set property 'error'");
        if (strlen(extError) > 0)
        {
            NAPI_CHECK( napi_create_string_utf8(env, extError, NAPI_AUTO_LENGTH, &extErrorText), "Could not create string extErrorText");
            NAPI_CHECK( napi_set_named_property(env, returnObj, "errorDetails", extErrorText), "Could not set property errorDetails");
        }
    }

    NAPI_CHECK( napi_create_array(env, &deviceArray), "Could not create array deviceArray");

    char **hints;
    /* Enumerate sound devices */
    err = snd_device_name_hint(-1, "pcm", (void***)&hints);
    if (err != 0)
    {
        strcpy(error, "cannot get device list: ");
        strcat(error, " - ");
        strcat(error, snd_strerror(err));

        handle_error();
        return returnObj;
    }

    napi_value deviceObj;
    char** n = hints;
    while (*n != NULL) {
        char *name = snd_device_name_get_hint(*n, "NAME");
        char *descr = snd_device_name_get_hint(*n, "DESC");
		char *io = snd_device_name_get_hint(*n, "IOID");

        //device name and napi object instantiation
        if(name != NULL) {
            status = napi_create_object(env, &deviceObj);

            napi_value deviceName;
            NAPI_CHECK( napi_create_string_utf8(env, name, NAPI_AUTO_LENGTH, &deviceName), "Could not create string deviceName");
            NAPI_CHECK( napi_set_named_property(env, deviceObj, "name", deviceName), "Could not set property 'name'");

            //device description
            if(descr != NULL){
                napi_value deviceDesc;
                NAPI_CHECK( napi_create_string_utf8(env, descr, NAPI_AUTO_LENGTH, &deviceDesc), "Could not create string deviceDesc");
                NAPI_CHECK( napi_set_named_property(env, deviceObj, "desc", deviceDesc), "Could not set property 'desc'");
            }

            //device io direction - One of "Bidirectional", "Input" or "Output"
            char *bio = io;
            if(bio == NULL)
                bio = "Bidirectional";
            napi_value deviceIo;
            NAPI_CHECK( napi_create_string_utf8(env, bio, NAPI_AUTO_LENGTH, &deviceIo), "Could not create string deviceIo");
            NAPI_CHECK( napi_set_named_property(env, deviceObj, "io", deviceIo), "Could not set property 'io'");

            //add device object to return array
            uint32_t index;
            NAPI_CHECK( napi_get_array_length(env, deviceArray, &index), "Could not get length of deviceArray");
            NAPI_CHECK( napi_set_element(env, deviceArray, index, deviceObj), "Could not append element to deviceArray");
        }

        if(name != NULL)
            free(name);
        if(descr != NULL)
            free(descr);
        if(io != NULL)
            free(io);
        n++;
    }

    //Free hint buffer too
    snd_device_name_free_hint((void**)hints);

    return deviceArray;
}

napi_value Init(napi_env env, napi_value exports)
{
    napi_status status;
    napi_value fn;

    NAPI_CHECK( napi_create_function(env, NULL, 0, GetCardInfo, NULL, &fn), "Unable to wrap native function");
    NAPI_CHECK( napi_set_named_property(env, exports, "get_card_info", fn), "Unable to populate exports");
    NAPI_CHECK( napi_create_function(env, NULL, 0, EnumeratePcmDevices, NULL, &fn), "Unable to wrap native function");
    NAPI_CHECK( napi_set_named_property(env, exports, "enumerate_cards", fn), "Unable to populate exports");

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
