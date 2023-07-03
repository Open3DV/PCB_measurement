# 电路板元器件高度检测

https://github.com/Open3DV/PCB_measurement/assets/3446495/ebdb13d8-7eac-4c86-b644-756c7488b683

本程序中3D数据采集功能使用的是XEMA开源工业3D相机（开源地址：https://github.com/open3dv/xema）

本程序使用到SIFT算子，需要依赖opencv4.5以上版本

由于算法中使用的是sift特征模版匹配，要求电路板上有足够的纹理特征以得到较好的匹配效果。另一个要求是电路板必须是类平面物体，否则用2D匹配会不准确

此电路板不能认为是类平面物体，因为元器件高度太高

![微信图片_20230703142455](https://github.com/Open3DV/PCB_measurement/assets/3446495/df30f646-3bd0-463f-bf57-5481deee6973)

此电路板可以认为是类平面物体

![微信图片_20230703142502](https://github.com/Open3DV/PCB_measurement/assets/3446495/ee9172eb-5563-4ab6-9600-ec2c40d4c3b3)

# 如何设置模板
首先需要准备一张电路板正面拍摄的图片。图片无畸变，切电路板四条边与图片的边重合。
![model](https://github.com/Open3DV/PCB_measurement/assets/3446495/71532eb6-a699-4bc3-82be-baab9c2ec81e)

接下来编辑model.xml文件，格式如下。reference_points即电路板平面的参考点。target_points即要检测的点。
这些点的XY坐标都是模板图片内的坐标。
```
<?xml version="1.0"?>
<opencv_storage>
    <reference_points>
        <rp1>
            <x> 380 </x>
            <y> 60 </y>
        </rp1>
        <rp2>
            <x> 380 </x>
            <y> 235 </y>
        </rp2>
        <rp3>
            <x> 97 </x>
            <y> 144 </y>
        </rp3>
    </reference_points>
    <target_points>
        <tp1>
            <x> 336 </x>
            <y> 65 </y>
        </tp1>
        <tp2>
            <x> 340 </x>
            <y> 179 </y>
        </tp2>
        <tp3>
            <x> 332 </x>
            <y> 217 </y>
        </tp3>
        <tp4>
            <x> 335 </x>
            <y> 268 </y>
        </tp4>
    </target_points>
</opencv_storage>
```
