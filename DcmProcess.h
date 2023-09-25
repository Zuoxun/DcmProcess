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
	
	// ����Dcmͼ�񷵻�����DicomImage��QString file_path	Ϊdcm�ļ���·��
	DicomImage* LoadDcmDataSet(QString filepath);

	//************************************************************************
	// ��������:    GetImageFromDcmDataSet	
	// ����˵��:	��DicomImage�л�ȡ���е�ͼ��֡��һ���DCM�ļ��У���
	// ��������: 	DicomImage * m_dcmImage		DCM�ļ���DicomImage����
	// �� �� ֵ:   	std::vector<cv::Mat>


	//************************************************************************
	vector<cv::Mat> GetImageFromDcmDataSet(DicomImage *m_dcmImage);

	//************************************************************************
	// ��������:    ShowImage
	// ����˵��:	��ȡָ��·����dcm�ļ�������ͼƬ��ʾ��16λ��
	// ��������: 	QString filepath	�����DCM�ļ���·��
	// �� �� ֵ:   	bool
	//************************************************************************
	bool ShowDcmImage(QString filepath);

	//************************************************************************
	// ��������:    GetDcmImage
	// ����˵��:	��ȡָ��·����dcm�ļ�����ò�����ͼ��
	// ��������: 	QString filepath	�����DCM�ļ���·��
	// �� �� ֵ:   	cv::Mat
	//************************************************************************
	cv::Mat GetDcmImage(QString filepath);


	//************************************************************************
	// ��������:    registerTag
	// ����˵��:	��������	��dcm�ļ���ָ��λ��ע����Ӧ��tag��ע��һ��tag��
	// ��������: 	int length			����ĳ���
	// ��������: 	UINT16 GroupNum		tag����ʼ���
	// ��������: 	UINT16 ElementNum	tag����ʼԪ�غ�
	// �� �� ֵ:   	void
	//************************************************************************
	void registerTag(int length, UINT16 GroupNum, UINT16 ElementNum);

	// �������� vector<string>ת��Ϊvector<QString>
	vector<string> QStrToStr(vector<QString> qstrs);

	//************************************************************************
	// ��������:    addTagValue
	// ����˵��:	��������	��dcm��ע����Զ���tag����Ӧ��ֵ������һ�����飬��һ��tag��ֵ��
	// ��������: 	DcmItem &item	����DcmDataset���ͣ�����ӵ�dcm�ļ�Dataset����
	// ��������: 	vector<QString> QValueArr		�洢��Ŀ��Ӧ��ֵ
	// ��������: 	UINT16 GroupNum		tag����ʼ���
	// ��������: 	UINT16 ElementNum	tag����ʼԪ�غ�
	// �� �� ֵ:   	void
	//************************************************************************
	void addTagValue(DcmItem &item, vector<QString> QValueArr, UINT16 GroupNum, UINT16 ElementNum);

	//************************************************************************
	// ��������:    registerTag
	// ����˵��:	��������	��dcm�ļ���ָ��λ��ע����Ӧ��tag��ע������tag��
	// ��������: 	int length			����ĳ���
	// ��������: 	UINT16 GroupNum1	��Ŀ����tag����ʼ���
	// ��������: 	UINT16 GroupNum2	��Ŀֵtag����ʼ���
	// ��������: 	UINT16 ElementNum1	��Ŀ����tag����ʼԪ�غ�
	// ��������: 	UINT16 ElementNum2	��Ŀֵtag����ʼԪ�غ�
	// �� �� ֵ:   	void
	//************************************************************************
	void registerTag(int length, UINT16 GroupNum1, UINT16 GroupNum2, UINT16 ElementNum1, UINT16 ElementNum2);

	//************************************************************************
	// ��������:    addTagValue
	// ����˵��:	��������	��dcm��ע����Զ���tag����Ӧ��ֵ�������������飬������tag��ֵ��
	// ��������: 	DcmItem &item	����DcmDataset���ͣ�����ӵ�dcm�ļ�Dataset����
	// ��������: 	vector<QString> QKeyArr	�洢��Ŀ���ƣ����磺��Ŀ���ƣ���ţ�
	// ��������: 	vector<QString> QValueArr	�洢��Ŀ��Ӧ��ֵ�����磺��Ŀֵ��T50��
	// ��������: 	UINT16 GroupNum1	��Ŀ����tag����ʼ���
	// ��������: 	UINT16 GroupNum2	��Ŀֵtag����ʼ���
	// ��������: 	UINT16 ElementNum1	��Ŀ����tag����ʼԪ�غ�
	// ��������: 	UINT16 ElementNum2	��Ŀֵtag����ʼԪ�غ�
	// �� �� ֵ:   	void
	//************************************************************************
	void addTagValue(DcmItem &item, vector<QString> QKeyArr, vector<QString> QValueArr, UINT16 GroupNum1, UINT16 GroupNum2, UINT16 ElementNum1, UINT16 ElementNum2);

	//************************************************************************
	// ��������:    writeMarkInforToDcmTag
	// ����˵��:	����ע��Ϣд��dcmTAG��
	// ��������: 	DcmItem &item		����DcmDataset���ͣ�����ӵ�dcm�ļ�Dataset����
	// ��������: 	vector<QString> QValueArr	�洢��ע��Ϣ������
	// ��������: 	UINT16 GroupNum		��ʼ���
	// ��������: 	UINT16 ElementNum	��ʼԪ�غ�
	// �� �� ֵ:   	bool
	//************************************************************************
	bool writeMarkInforToDcmTag(DcmItem &item, vector<QString> ValueArr, UINT16 GroupNum, UINT16 ElementNum);

	//************************************************************************
	// ��������:    writeWorkInforToDcmTag
	// ����˵��:	��������Ϣд��dcmTAG��
	// ��������: 	DcmItem &item	����DcmDataset���ͣ�����ӵ�dcm�ļ�Dataset����
	// ��������: 	vector<QString> QKeyArr	������Ϣ���ƣ����磺��Ŀ���ƣ���ţ�
	// ��������: 	vector<QString> QValueArr	������Ϣ��Ӧ��ֵ�����磺��Ŀֵ��T50��
	// ��������: 	UINT16 GroupNum1	��Ŀ����tag����ʼ���
	// ��������: 	UINT16 GroupNum2	��Ŀֵtag����ʼ���
	// ��������: 	UINT16 ElementNum1	��Ŀ����tag����ʼԪ�غ�
	// ��������: 	UINT16 ElementNum2	��Ŀֵtag����ʼԪ�غ�
	// �� �� ֵ:   	bool
	//************************************************************************
	bool writeWorkInforToDcmTag(DcmItem &item, vector<QString> QKeyArr, vector<QString> QValueArr, UINT16 GroupNum1, UINT16 ElementNum1, UINT16 GroupNum2, UINT16 ElementNum2);

	//************************************************************************
	// ��������:    WriteMat16BitToDcm
	// ����˵��:	��16λ��Mat���浽һ���½���dcm�ļ��У�������Ϣ��ӵ�dcmTAG��
	// ��������: 	cv::Mat img16Bit	��Ҫ�����16λMat
	// ��������: 	string outputPath	���dcm�ļ�·��
	// ����������	vector�ֱ𱣴湤����Ϣ��������Ϣ����ע��Ϣ
	// �� �� ֵ:   	bool
	//************************************************************************
	bool SaveImgAsDcm(cv::Mat img16Bit, QString QoutputPath, vector<QString> QworkInforKeyArr, vector<QString> QworkInforValueArr,
		vector<QString> QimagingKeyArr, vector<QString> QimagingValueArr, vector<QString> QmarkInforValueArr);



	//************************************************************************
	// ����˵��:	�����ĸ��������ܣ���ȡdcm�ļ���ͼ�������Ϣ��������Ϣ��������Ϣ����ע��Ϣ
	// ��������: 	QString dcmPath		dcm�ļ�·��
	// ��������: 	vector<QString>		��Ӧ��KeyArr��������Ŀ����
	// ����������	vector<QString>		��Ӧ��ValueArr��������Ŀֵ
	// �� �� ֵ:   	void
	//************************************************************************
	void getImgBaseInfor(QString dcmPath, vector<QString> &imgBaseInforKeyArr, vector<QString> &imgBaseInforValueArr);
	void getImagingInfor(QString dcmPath, vector<QString> &imagingInforKeyArr, vector<QString> &imagingInforValueArr);
	void getWorkInfor(QString dcmPath, vector<QString> &workInforKeyArr, vector<QString> &workInforValueArr);
	void getMarkInfor(QString dcmPath, vector<QString> &markInforArr);
	
	//************************************************************************
	// ��������:	FourIntToMarkString
	// ����˵��:	��ע����ϸ��Ϣ���ĸ������Լ�����ת��ΪQString����
	// ��������: 	int type	��ע�����ࡣ1Ϊ���֣�2Ϊֱ�ߣ�3Ϊ���Σ�4ΪԲ
	// ��������: 	int x, int y, int w, int h
	// ��������: 	QString text ���ֲ��֣�����Ϊ��
	// �� �� ֵ:   	QString		����ת���õ����ַ���
	//************************************************************************
	QString FourIntToMarkString(int type, int x, int y, int w, int h, QString text = "");

	//************************************************************************
	// ��������:	MarkStrToFourInt
	// ����˵��:	��ע��Ϣ����QString���ַ��������������ĸ�����������
	// ��������: 	QString originalStr		��ʾ��ע��Ϣ�����ַ���
	// ��������: 	int &type,int &x, int &y, int &w, int &h	���ձ�ע���͡�xywh��Ϣ
	// �� �� ֵ:   	QString		���ر�ע��Ϣ�е�����
	//************************************************************************
	QString MarkStrToFourInt(QString originalStr, int &type, int &x, int &y, int &w, int &h);



	//************************************************************************
	// ��������:    PrintAllTag
	// ����˵��:	���ָ��dcm�ļ���tag���ƣ�VR����ţ�Ԫ�غ��Լ�ʵ�ʵ�ֵ
	// ��������: 	QString dcmfilepath		ԭʼdcm�ļ���·��
	// �� �� ֵ:   	void
	//************************************************************************
	void PrintAllTag(QString dcmfilepath);


};

