#!/usr/bin/env lua
---------------------------------------------------------------------------------------------------
-- FileName     : config_function.lua
--
--
--
--
--
--
---------------------------------------------------------------------------------------------------

_G_THIS_FILE=[[config_function.lua]]
_G_THIS_FILE_GUID="2A719C2E-3230-4A68-95F1-A891292AD058"

_G_SUPPORT_LIST={}

category_device=1
category_stream=2

--========================================================================
string_key_word=1 --"string"
function device_string_config(key,value)
    return
    {
        category    =category_device,
        type_name   =string_key_word,
        key_name    =tostring(key),
        key_value   =tostring(value)
    }
end

function stream_string_config(key,value)
    return
    {
        category    =category_stream,
        type_name   =string_key_word,
        key_name    =tostring(key),
        key_value   =tostring(value)
    }
end


--========================================================================
integer_key_word=2 --"int32_t"

function device_int32_config(key,value)
    return
    {
        category    =category_device,
        type_name   =integer_key_word,
        key_name    =tostring(key),
        key_value   =tonumber(value)
    }
end

function stream_int32_config(key,value)
    return
    {
        category    =category_stream,
        type_name   =integer_key_word,
        key_name    =tostring(key),
        key_value   =tonumber(value)
    }
end

--========================================================================
long_integer_key_word=3 --"int64_t"
function device_int64_config(key,value)
    return
    {
        category    =category_device,
        type_name   =long_integer_key_word,
        key_name    =tostring(key),
        key_value   =tonumber(value)
    }
end

function stream_int64_config(key,value)
    return
    {
        category    =category_stream,
        type_name   =long_integer_key_word,
        key_name    =tostring(key),
        key_value   =tonumber(value)
    }
end

--========================================================================
double_key_word=4 --"double"
function device_double_config(key,value)
    return
    {
        category    =category_device,
        type_name   =double_key_word,
        key_name    =tostring(key),
        key_value   =tonumber(value)
    }
end

function stream_double_config(key,value)
    return
    {
        category    =category_stream,
        type_name   =double_key_word,
        key_name    =tostring(key),
        key_value   =tonumber(value)
    }
end

--========================================================================
command_key_word=5 --"command"
function device_command_config(key,value)
    return
    {
        category    =category_device,
        type_name   =command_key_word,
        key_name    =tostring(key),
        key_value   =nil
    }
end

function stream_command_config(key,value)
    return
    {
        category    =category_stream,
        type_name   =command_key_word,
        key_name    =tostring(key),
        key_value   =nil
    }
end
--[[
test=
{
    device_string_config("device_string","device_string_value"),
    device_int32_config("device_int32_key",32),
    device_int64_config("device_int64_key",64),
    device_double_config("device_double_key",199.123456),
    device_command_config("device_command_key","device_command_value"),

    stream_string_config("stream_string","device_string_value"),
    stream_int32_config("stream_int32_key",32),
    stream_int64_config("stream_int64_key",64),
    stream_double_config("stream_double_key",199.123456),
    stream_command_config("stream_command_key","stream_command_value")
}
]]

load_config=
{
    --max_index=0
}

unload_config={}

restore_config_set_device=
{
    device_string_config("PixelFormat","RGB8Packed")
    --device_string_config("RGB24bits","Bit_Allocation")
}

restore_config_set_stream=
{
    stream_int32_config("FirstPacketTimeout",0)
}

open={}

close={}

set_buffer={}

unset_buffer={}

set_current={}

get_current={}

set_whitebalance={}

get_whitebalance={}

set_callback={}

unset_callback={}

set_trigger_free=
{
    device_string_config("DeviceScanType","Linescan")
    --device_string_config("Internal","Trigger_Origin")
}

set_trigger_hardware=
{
    device_string_config("DeviceScanType","Areascan")
    --device_string_config("External","Trigger_Origin")
    --device_string_config("Camera_link","Trigger_Input")
    --device_string_config("Active_Low","Trigger_Polarity")
}

set_width=
{
    device_int32_config("Width",2048)
}

get_width={}

set_height=
{
    device_int32_config("Height",2500)
}

get_height={}

set_exposure_abs=
{
    --device_string_config("Shutter_Select","Trigger_Mode")
    --device_string_config("Programmable_Exposure_Red",2100)
    --device_string_config("Programmable_Exposure_Green",2100)
    --device_string_config("Programmable_Exposure_Blue",2100)
}

get_exposure_abs={}

grab={}

stop={}

