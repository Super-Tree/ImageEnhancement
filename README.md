## 说明
解决方案包含四个工程  
+ dehazing 
+ imgHDR 
+ PicEstimation 
+ retinexEnhance  

其中主体程序和架构在dehazing工程中，调用imgHDR和PicEstimation生成的lib静态库完成 retinex算法的调用和图像质量评估算法
