#pragma once
#pragma execution_character_set("utf-8")
#include <iostream>
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcobject.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmimage/diregist.h>
#include <dcmtk/dcmdata/dcrledrg.h>
#include <dcmtk/dcmjpeg/djdecode.h>
#include <qstring.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include "qdebug.h"
#include "qstring.h"


using namespace std;
typedef unsigned short      UINT16, *PUINT16;

class DcmProcess
{
public:
	unsigned short Img_bitCount;
	
	// 加载Dcm图像返回它的DicomImage，QString file_path	为dcm文件的路径
	DicomImage* LoadDcmDataSet(QString filepath);

	//************************************************************************
	// 函数名称:    GetImageFromDcmDataSet	
	// 函数说明:	从DicomImage中获取所有的图像帧（一般的DCM文件中，）
	// 函数参数: 	DicomImage * m_dcmImage		DCM文件的DicomImage对象
	// 返 回 值:   	std::vector<cv::Mat>


	//************************************************************************
	vector<cv::Mat> GetImageFromDcmDataSet(DicomImage *m_dcmImage);

	//************************************************************************
	// 函数名称:    ShowImage
	// 函数说明:	读取指定路径的dcm文件，并将图片显示（16位）
	// 函数参数: 	QString filepath	保存的DCM文件的路径
	// 返 回 值:   	bool
	//************************************************************************
	bool ShowDcmImage(QString filepath);

	//************************************************************************
	// 函数名称:    GetDcmImage
	// 函数说明:	读取指定路径的dcm文件，获得并返回图像
	// 函数参数: 	QString filepath	保存的DCM文件的路径
	// 返 回 值:   	cv::Mat
	//************************************************************************
	cv::Mat GetDcmImage(QString filepath);


	//************************************************************************
	// 函数名称:    registerTag
	// 函数说明:	辅助函数	在dcm文件的指定位置注册相应的tag（注册一组tag）
	// 函数参数: 	int length			数组的长度
	// 函数参数: 	UINT16 GroupNum		tag的起始组号
	// 函数参数: 	UINT16 ElementNum	tag的起始元素号
	// 返 回 值:   	void
	//************************************************************************
	void registerTag(int length, UINT16 GroupNum, UINT16 ElementNum);

	// 辅助函数 vector<string>转化为vector<QString>
	vector<string> QStrToStr(vector<QString> qstrs);

	//************************************************************************
	// 函数名称:    addTagValue
	// 函数说明:	辅助函数	给dcm中注册的自定义tag赋相应的值（传入一个数组，给一组tag赋值）
	// 函数参数: 	DcmItem &item	传入DcmDataset类型，待添加的dcm文件Dataset部分
	// 函数参数: 	vector<QString> QValueArr		存储条目对应的值
	// 函数参数: 	UINT16 GroupNum		tag的起始组号
	// 函数参数: 	UINT16 ElementNum	tag的起始元素号
	// 返 回 值:   	void
	//************************************************************************
	void addTagValue(DcmItem &item, vector<QString> QValueArr, UINT16 GroupNum, UINT16 ElementNum);

	//************************************************************************
	// 函数名称:    registerTag
	// 函数说明:	辅助函数	在dcm文件的指定位置注册相应的tag（注册两组tag）
	// 函数参数: 	int length			数组的长度
	// 函数参数: 	UINT16 GroupNum1	条目名称tag的起始组号
	// 函数参数: 	UINT16 GroupNum2	条目值tag的起始组号
	// 函数参数: 	UINT16 ElementNum1	条目名称tag的起始元素号
	// 函数参数: 	UINT16 ElementNum2	条目值tag的起始元素号
	// 返 回 值:   	void
	//************************************************************************
	void registerTag(int length, UINT16 GroupNum1, UINT16 GroupNum2, UINT16 ElementNum1, UINT16 ElementNum2);

	//************************************************************************
	// 函数名称:    addTagValue
	// 函数说明:	辅助函数	给dcm中注册的自定义tag赋相应的值（传入两个数组，给两组tag赋值）
	// 函数参数: 	DcmItem &item	传入DcmDataset类型，待添加的dcm文件Dataset部分
	// 函数参数: 	vector<QString> QKeyArr	存储条目名称（例如：条目名称，编号）
	// 函数参数: 	vector<QString> QValueArr	存储条目对应的值（例如：条目值，T50）
	// 函数参数: 	UINT16 GroupNum1	条目名称tag的起始组号
	// 函数参数: 	UINT16 GroupNum2	条目值tag的起始组号
	// 函数参数: 	UINT16 ElementNum1	条目名称tag的起始元素号
	// 函数参数: 	UINT16 ElementNum2	条目值tag的起始元素号
	// 返 回 值:   	void
	//************************************************************************
	void addTagValue(DcmItem &item, vector<QString> QKeyArr, vector<QString> QValueArr, UINT16 GroupNum1, UINT16 GroupNum2, UINT16 ElementNum1, UINT16 ElementNum2);

	//************************************************************************
	// 函数名称:    writeMarkInforToDcmTag
	// 函数说明:	将标注信息写入dcmTAG中
	// 函数参数: 	DcmItem &item		传入DcmDataset类型，待添加的dcm文件Dataset部分
	// 函数参数: 	vector<QString> QValueArr	存储标注信息的数组
	// 函数参数: 	UINT16 GroupNum		起始组号
	// 函数参数: 	UINT16 ElementNum	起始元素号
	// 返 回 值:   	bool
	//************************************************************************
	bool writeMarkInforToDcmTag(DcmItem &item, vector<QString> ValueArr, UINT16 GroupNum, UINT16 ElementNum);

	//************************************************************************
	// 函数名称:    writeWorkInforToDcmTag
	// 函数说明:	将工作信息写入dcmTAG中
	// 函数参数: 	DcmItem &item	传入DcmDataset类型，待添加的dcm文件Dataset部分
	// 函数参数: 	vector<QString> QKeyArr	工作信息名称（例如：条目名称，编号）
	// 函数参数: 	vector<QString> QValueArr	工作信息对应的值（例如：条目值，T50）
	// 函数参数: 	UINT16 GroupNum1	条目名称tag的起始组号
	// 函数参数: 	UINT16 GroupNum2	条目值tag的起始组号
	// 函数参数: 	UINT16 ElementNum1	条目名称tag的起始元素号
	// 函数参数: 	UINT16 ElementNum2	条目值tag的起始元素号
	// 返 回 值:   	bool
	//************************************************************************
	bool writeWorkInforToDcmTag(DcmItem &item, vector<QString> QKeyArr, vector<QString> QValueArr, UINT16 GroupNum1, UINT16 ElementNum1, UINT16 GroupNum2, UINT16 ElementNum2);

	//************************************************************************
	// 函数名称:    WriteMat16BitToDcm
	// 函数说明:	将16位的Mat保存到一个新建的dcm文件中，并将信息添加到dcmTAG中
	// 函数参数: 	cv::Mat img16Bit	需要保存的16位Mat
	// 函数参数: 	string outputPath	输出dcm文件路径
	// 函数参数：	vector分别保存工作信息、成像信息、标注信息
	// 返 回 值:   	bool
	//************************************************************************
	bool SaveImgAsDcm(cv::Mat img16Bit, QString QoutputPath, vector<QString> QworkInforKeyArr, vector<QString> QworkInforValueArr,
		vector<QString> QimagingKeyArr, vector<QString> QimagingValueArr, vector<QString> QmarkInforValueArr);



	//************************************************************************
	// 函数说明:	以下四个函数功能：读取dcm文件中图像基本信息、成像信息、工作信息、标注信息
	// 函数参数: 	QString dcmPath		dcm文件路径
	// 函数参数: 	vector<QString>		相应的KeyArr，保存条目名称
	// 函数参数：	vector<QString>		相应的ValueArr，保存条目值
	// 返 回 值:   	void
	//************************************************************************
	void getImgBaseInfor(QString dcmPath, vector<QString> &imgBaseInforKeyArr, vector<QString> &imgBaseInforValueArr);
	void getImagingInfor(QString dcmPath, vector<QString> &imagingInforKeyArr, vector<QString> &imagingInforValueArr);
	void getWorkInfor(QString dcmPath, vector<QString> &workInforKeyArr, vector<QString> &workInforValueArr);
	void getMarkInfor(QString dcmPath, vector<QString> &markInforArr);
	
	//************************************************************************
	// 函数名称:	FourIntToMarkString
	// 函数说明:	标注的详细信息，四个整数以及文字转化为QString类型
	// 函数参数: 	int type	标注的种类。1为文字；2为直线；3为矩形；4为圆
	// 函数参数: 	int x, int y, int w, int h
	// 函数参数: 	QString text 文字部分，可以为空
	// 返 回 值:   	QString		返回转化好的总字符串
	//************************************************************************
	QString FourIntToMarkString(int type, int x, int y, int w, int h, QString text = "");

	//************************************************************************
	// 函数名称:	MarkStrToFourInt
	// 函数说明:	标注信息，从QString的字符串，解析出来四个整数和文字
	// 函数参数: 	QString originalStr		表示标注信息的总字符串
	// 函数参数: 	int &type,int &x, int &y, int &w, int &h	接收标注类型、xywh信息
	// 返 回 值:   	QString		返回标注信息中的文字
	//************************************************************************
	QString MarkStrToFourInt(QString originalStr, int &type, int &x, int &y, int &w, int &h);



	//************************************************************************
	// 函数名称:    PrintAllTag
	// 函数说明:	输出指定dcm文件的tag名称，VR，组号，元素号以及实际的值
	// 函数参数: 	QString dcmfilepath		原始dcm文件的路径
	// 返 回 值:   	void
	//************************************************************************
	void PrintAllTag(QString dcmfilepath);


};

