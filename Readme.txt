[基础工程框架]

├─Build				临时文件夹，所有工程编译临时文件输出到此文件夹下的项目名文件夹下
├─Debug				Debug版程序
├─Doc				文档
├─Include			一些无需编译引用的头文件
├─Lib				无需编译使用的库文件
├─Release			Release版程序
├─Res				程序资源，编译结束后完整拷贝至Debug和Release目录
├─Sln				项目
├─Src				项目源码
│  └─3rdParty		第三方开源项目源码
│  ├─Framework		基础App框架
│  └─Plugin			业务逻辑服务模块
└─Tools				使用的工具程序和脚本

VisualStudio设置
1.防止Sln文件夹下生成 .sdf和ipch文件夹
工具-》选项-》文本编辑器-》C/C++ -》高级-》回退位置
将 【始终使用回退位置】和【回退位置已在使用时，不警告】设置为True
#该修改会将sdf等文件存储到系统默认临时文件夹下，使用电脑管家等清理垃圾会被删除导致重建

[项目设置]

1.exe和dll项目设置参考
常规：
1.输出目录：$(SolutionDir)..\$(Configuration)\
2.中间目录：$(SolutionDir)..\Build\$(ProjectName)\$(Configuration)\
3.目标文件名：$(ProjectName)

2.静态库项目设置参考
常规：
1.输出目录：$(SolutionDir)..\Lib\$(Configuration)\
2.中间目录：$(SolutionDir)..\Build\$(ProjectName)\$(Configuration)\
3.目标文件名：$(ProjectName)

3.附加包含目录设置
$(SolutionDir)..\Src\[ProjectName]

4.附加库目录设置
$(SolutionDir)..\Lib\$(Configuration)

5.动态链接库Lib路径设置
链接器-》所有选项
导入库：$(SolutionDir)..\Lib\$(Configuration)\$(ProjectName).lib