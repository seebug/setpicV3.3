#!/usr/bin/env lua

on,ON=true,true

off,OFF=false,false

--获取CPU序列号
--ID: A9 06 03 00 FF FB EB BF
function get_system_cpu_id(...)
    local ret=""
    local reg="%s*ID:"
    for _index=1,8,1 do
        reg=reg .. "%s*(%w+)"
    end
    local obj=io.popen("dmidecode -t processor | grep \"ID\"","r")
    for l in obj:lines() do
        local _,_,_1,_2,_3,_4,_5,_6,_7,_8=string.find(string.upper(l),reg)
        if(_ and _ and _1 and _2 and _3 and _4 and _5 and _6 and _7 and _8) then
            ret=ret .. _1 .. _2 .. _3 .. _4 .. _5 .. _6 .. _7 .. _8
        end
    end
    if(obj) then
        obj:close()
    end
    return ret
end

--自动分析ping的，如果 ping 通过，则说明网络层畅通，返回1，否则说明网络不通，返回0
function get_auto_ping_result( ip_address)
    local obj=io.popen("ping " .. tostring(ip_address) .. " -c 1 ")
    local reg="time=(.*)ms"
    local ret;
    for l in obj:lines() do
        local _,_,_time=string.find(string.lower(l),reg);
        if(_time and tonumber(_time)>0) then
            ret=1
        else
            ret=0
        end
    end
    if(obj) then
        obj:close()
    end
    return ret;
end

--是否检查系统信息
checksysteminfo =true

--是否检查系统加密狗
checksystemdog  =true

--libencryption.so 文件不存在时，是否直接退出
exit_noencryption=true

--在不传递任何有关曝光时间参数的情况下，系统启动时读取的曝光时间
default_exposuretimeabs=2986

--在不传递任何有关增益参数的情况下，系统启动时读取的增益参数
default_gain=118

--在不传递任何有关RGB增益参数的情况下，系统启动时读取的RGB增益
default_rgain,default_ggain,default_bgain=5667,4096,5200

--default_middle_rect={180,20,1440,1216}

--default_mid_point_sx,default_mid_point_sy=180,20

--default_mid_point_ex,default_mid_point_ey=1440,1216

--[[FOR CREATE CALIBRATE2D.XML]]

--打开或关闭去畸变
--IF NOT CALIBRATE ,SET TO :off
calibrate2d_switch=on

--去畸变时使用的标定板图像数量
calibrate2d_board_image_count=20

--内角点数量
--board Size(8,10)
calibrate2d_board_size_x,calibrate2d_board_size_y=11,8

--角点之间的距离
--setSquareSize
calibrate2d_board_point_to_point=30

--要求setpic使用的用于存取标定板图像的路径
calibrate2d_board_input_path_filestring=[[./runtime/data/board/]]

--调用生成标定XML时，XML文件的路径及文件名
calibrate2d_xml_output_path_filestring=[[./runtime/data/calibrate2d.xml]]

tb_pattern={"NOT_EXISTING", "CHESSBOARD", "CIRCLES_GRID", "ASYMMETRIC_CIRCLES_GRID"}

calibrate2d_pattern=tb_pattern[2] --"CHESSBOARD"

tb_calibtype={"CALIB_INVALID","INTRINSICS","EXTRINSICS","OTHER"}

calibrate2d_calibrate_type=tb_calibtype[2] --"INTRINSICS"

calibrate2d_viewrectify_type=tb_calibtype[3] --"EXTRINSICS"
--[[
function get_calibrate2d_calibrate_type(...)
    local file =io.open(tostring(calibrate2d_xml_output_path_filestring))
    if(file) then
        file:close()
        return tb_calibtype[3] --"EXTRINSICS"
    else
        return tb_calibtype[2] --"INTRINSICS"
    end
end

calibrate2d_calibrate_type=get_calibrate2d_calibrate_type()

]]

--ViewRectify SWITCH : ON / OFF
calibrate2d_ViewRectify_switch=on

calibrate2d_ViewRectify_file_pathstring=[[./runtime/data/viewrectify.xml]]

function get_calibrate2d_view_rectify_done( ... )
    local file =io.open(tostring(calibrate2d_ViewRectify_file_pathstring))
    if(file) then
        file:close()
        return true
    else
        return false
    end
end

calibrate2d_ViewRectify_done=get_calibrate2d_view_rectify_done()

--[[
the end of script config
]]

if(true==checksysteminfo) then
    if("C3060300FFFBEBBF"==string.upper(get_system_cpu_id())) then
        checksysteminfo=false
    end
end

if(true==checksystemdog) then
    if("C3060300FFFBEBBF"==string.upper(get_system_cpu_id())) then
        checksystemdog=false;
    end
end

if(true==exit_noencryption) then
    if("C3060300FFFBEBBF"==string.upper(get_system_cpu_id())) then
        exit_noencryption=false;
    end
end
