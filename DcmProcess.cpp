#include "DcmProcess.h"


DicomImage* DcmProcess::LoadDcmDataSet(QString filepath) {

	DcmFileFormat fileformat;

	OFCondition oc = fileformat.loadFile(filepath.toStdString().c_str());
	if (!oc.good()) {
		cout << "file load error" << endl;
		return nullptr;
	}
	DcmDataset *dataset = fileformat.getDataset();
	E_TransferSyntax xfer = dataset->getOriginalXfer();

	//获得传输语法字符串 
	const char* transferSyntax = NULL;
	fileformat.getMetaInfo()->findAndGetString(DCM_TransferSyntaxUID, transferSyntax);

	string losslessTransUID = "1.2.840.10008.1.2.4.70";
	string lossTransUID = "1.2.840.10008.1.2.4.51";
	string losslessP14 = "1.2.840.10008.1.2.4.57";
	string lossyP1 = "1.2.840.10008.1.2.4.50";
	string lossyRLE = "1.2.840.10008.1.2.5";
	if (transferSyntax == losslessTransUID || transferSyntax == lossTransUID ||
		transferSyntax == losslessP14 || transferSyntax == lossyP1)
	{
		DJDecoderRegistration::registerCodecs();
		//对压缩的图像像素进行解压
		dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
		DJDecoderRegistration::cleanup();
	}
	else if (transferSyntax == lossyRLE)
	{
		DcmRLEDecoderRegistration::registerCodecs();
		dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
		DcmRLEDecoderRegistration::cleanup();
	}
	else
	{
		dataset->chooseRepresentation(xfer, NULL);
	}

	//利用dataset生成DicomImage，需要上面的解压方法； 
	DicomImage* m_dcmImage = new DicomImage((DcmObject*)dataset, xfer);

	return m_dcmImage;

}

vector<cv::Mat> DcmProcess::GetImageFromDcmDataSet(DicomImage *m_dcmImage) {
	//输出图像向量
	vector<cv::Mat> output_img;
	//获取这个文件包含的图像的帧数
	int framecount(m_dcmImage->getFrameCount());
	cout << "Dcm文件中一共有几帧图像，framecount is:" << framecount << endl;
	for (int k = 0; k < framecount; k++)
	{
		unsigned short *pixelData = (unsigned short*)(m_dcmImage->getOutputData(16, k, 0)); //获得16位的图像数据指针  
		if (pixelData != NULL)
		{
			int m_height = m_dcmImage->getHeight();
			int m_width = m_dcmImage->getWidth();
			// 1表示灰度图像，3表示彩色图像
			// 这里设定默认为16为灰度图像
			this->Img_bitCount = 1;
			if (3 == this->Img_bitCount)
			{
				cout << "通道数为3" << endl;
				cv::Mat dst2(m_height, m_width, CV_16UC3, cv::Scalar::all(0));
				for (int i = 0; i < m_height; i++)
				{
					for (int j = 0; j < m_width; j++)
					{
						dst2.at<cv::Vec3b>(i, j)[0] = *(pixelData + i * m_width * 3 + j * 3 + 2);   //B channel  
						dst2.at<cv::Vec3b>(i, j)[1] = *(pixelData + i * m_width * 3 + j * 3 + 1);   //G channel  
						dst2.at<cv::Vec3b>(i, j)[2] = *(pixelData + i * m_width * 3 + j * 3);       //R channel  
					}
				}
				output_img.push_back(dst2);
				/*cv::imshow("image", dst2);
				cv::waitKey(0);*/
			}
			else if (1 == this->Img_bitCount)
			{
				cout << "通道数为1" << endl;
				cv::Mat dst2(m_height, m_width, CV_16UC1, cv::Scalar::all(0));
				ushort* data = nullptr;
				for (int i = 0; i < m_height; i++)
				{
					data = dst2.ptr<ushort>(i);
					for (int j = 0; j < m_width; j++)
					{
						data[j] = *(pixelData + i * m_width + j);
					}
				}
				output_img.push_back(dst2);
				/*cv::imshow("image", dst2);
				cv::waitKey(0);*/
			}

		}
	}

	return output_img;

}

bool DcmProcess::ShowDcmImage(QString filepath)
{
	if (!filepath.length())
	{
		cout << "No parent dcm filepath! error" << endl;
		return false;
	}

	DcmFileFormat fileformat;
	//读取Dicom图像
	OFCondition oc = fileformat.loadFile(filepath.toStdString().c_str());
	//判断Dicom文件是否读取成功 
	if (!oc.good())
	{
		std::cout << "file Load error" << std::endl;
		return false;
	}
	DcmDataset *dataset = fileformat.getDataset();                              //得到Dicom的数据集  
	E_TransferSyntax xfer = dataset->getOriginalXfer();                          //得到传输语法  


	 //1表示灰色图像；3表示彩色图像
	unsigned short img_bits(0);
	dataset->findAndGetUint16(DCM_SamplesPerPixel, img_bits);

	unsigned short m_width;                                                     //获取图像的窗宽高  
	unsigned short m_height;
	dataset->findAndGetUint16(DCM_Rows, m_height);
	dataset->findAndGetUint16(DCM_Columns, m_width);


	//获得传输语法字符串 
	const char* transferSyntax = NULL;
	fileformat.getMetaInfo()->findAndGetString(DCM_TransferSyntaxUID, transferSyntax);

	//输出相应的信息
	/*cout << "img_bits is:" << img_bits << endl;
	cout << "宽，高分别是：" << m_width << " " << m_height << endl;
	cout << "传输语法字符串是：" << transferSyntax << endl;*/

	string losslessTransUID = "1.2.840.10008.1.2.4.70";
	string lossTransUID = "1.2.840.10008.1.2.4.51";
	string losslessP14 = "1.2.840.10008.1.2.4.57";
	string lossyP1 = "1.2.840.10008.1.2.4.50";
	string lossyRLE = "1.2.840.10008.1.2.5";
	if (transferSyntax == losslessTransUID || transferSyntax == lossTransUID ||
		transferSyntax == losslessP14 || transferSyntax == lossyP1)
	{
		DJDecoderRegistration::registerCodecs();
		dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);                       //对压缩的图像像素进行解压  
		DJDecoderRegistration::cleanup();
	}
	else if (transferSyntax == lossyRLE)
	{
		DcmRLEDecoderRegistration::registerCodecs();
		dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
		DcmRLEDecoderRegistration::cleanup();
	}
	else
	{
		dataset->chooseRepresentation(xfer, NULL);
	}

	DcmElement* element = NULL;
	OFCondition result = dataset->findAndGetElement(DCM_PixelData, element); //获取图像数据  
	if (result.bad() || element == NULL)
		return false;
	//const Uint8* image_data = nullptr;
	//dataset->findAndGetUint8Array(DCM_PixelData, image_data);
	const Uint16* image16_data = nullptr;
	dataset->findAndGetUint16Array(DCM_PixelData, image16_data);
	if (image16_data != NULL)
	{
		if (1 == img_bits)
		{
			//cv::Mat dst(m_height, m_width, CV_8UC1, cv::Scalar::all(0));
			//unsigned char* data = nullptr;
			//for (int i = 0; i < m_height; i++)
			//{
			//	data = dst.ptr<unsigned char>(i); //取得每一行的头指针
			//	for (int j = 0; j < m_width; j++)
			//	{
			//		*data++ = image_data[i*m_width + j];
			//	}
			//}
			//16位灰度图显示
			cv::Mat dst = cv::Mat(m_height, m_width, CV_16UC1, cv::Scalar::all(0));
			unsigned short* data = nullptr;
			for (int i = 0; i < m_height; i++)
			{
				for (int j = 0; j < m_width; j++)
				{
					dst.at<unsigned short>(i, j) = image16_data[i*m_width + j];
				}
			}
			cv::imshow("gray", dst);
			cv::waitKey(0);
		}
		else if (3 == img_bits)
		{
			//cv::Mat dst(m_height, m_width, CV_8UC3, cv::Scalar::all(0));
			//unsigned short* data = nullptr;
			//for (int i = 0; i < m_height; i++)
			//{
			//	for (int j = 0; j < m_width; j++)
			//	{
			//		dst.at<cv::Vec3b>(i, j)[0] = image_data[i*m_width * 3 + j * 3 + 2];   //B  
			//		dst.at<cv::Vec3b>(i, j)[1] = image_data[i*m_width * 3 + j * 3 + 1];   //G 
			//		dst.at<cv::Vec3b>(i, j)[2] = image_data[i*m_width * 3 + j * 3];       //R 
			//	}
			//}
			//16位3通道图显示
			cv::Mat dst(m_height, m_width, CV_16UC3, cv::Scalar::all(0));
			unsigned short* data = nullptr;
			for (int i = 0; i < m_height; i++)
			{
				for (int j = 0; j < m_width; j++)
				{
					dst.at<cv::Vec3b>(i, j)[0] = image16_data[i*m_width * 3 + j * 3 + 2];   //B 
					dst.at<cv::Vec3b>(i, j)[1] = image16_data[i*m_width * 3 + j * 3 + 1];   //G  
					dst.at<cv::Vec3b>(i, j)[2] = image16_data[i*m_width * 3 + j * 3];       //R 
				}
			}
			cv::imshow("colored", dst);
			cv::waitKey(0);
		}
	}
	return true;
}


cv::Mat DcmProcess::GetDcmImage(QString filepath)
{
	DcmFileFormat fileformat;
	//读取Dicom图像
	//第一种将QString转成const char,中文会出错，无法读出文件
	//OFCondition oc = fileformat.loadFile(filepath.toStdString().c_str());
	OFCondition oc = fileformat.loadFile(std::string((const char*)filepath.toLocal8Bit().constData()).c_str());
	//判断Dicom文件是否读取成功 
	if (!oc.good())
	{
		qDebug() << "file Load error" << endl;
	}
	DcmDataset *dataset = fileformat.getDataset();                              //得到Dicom的数据集  
	E_TransferSyntax xfer = dataset->getOriginalXfer();                          //得到传输语法  


	 //1表示灰色图像；3表示彩色图像
	unsigned short img_bits(0);
	dataset->findAndGetUint16(DCM_SamplesPerPixel, img_bits);

	unsigned short m_width;                                                     //获取图像的窗宽高  
	unsigned short m_height;
	dataset->findAndGetUint16(DCM_Rows, m_height);
	dataset->findAndGetUint16(DCM_Columns, m_width);


	//获得传输语法字符串 
	const char* transferSyntax = NULL;
	fileformat.getMetaInfo()->findAndGetString(DCM_TransferSyntaxUID, transferSyntax);

	//输出相应的信息
	/*cout << "img_bits is:" << img_bits << endl;
	cout << "宽，高分别是：" << m_width << " " << m_height << endl;
	cout << "传输语法字符串是：" << transferSyntax << endl;*/

	string losslessTransUID = "1.2.840.10008.1.2.4.70";
	string lossTransUID = "1.2.840.10008.1.2.4.51";
	string losslessP14 = "1.2.840.10008.1.2.4.57";
	string lossyP1 = "1.2.840.10008.1.2.4.50";
	string lossyRLE = "1.2.840.10008.1.2.5";
	if (transferSyntax == losslessTransUID || transferSyntax == lossTransUID ||
		transferSyntax == losslessP14 || transferSyntax == lossyP1)
	{
		DJDecoderRegistration::registerCodecs();
		dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);                       //对压缩的图像像素进行解压  
		DJDecoderRegistration::cleanup();
	}
	else if (transferSyntax == lossyRLE)
	{
		DcmRLEDecoderRegistration::registerCodecs();
		dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
		DcmRLEDecoderRegistration::cleanup();
	}
	else
	{
		dataset->chooseRepresentation(xfer, NULL);
	}

	DcmElement* element = NULL;
	OFCondition result = dataset->findAndGetElement(DCM_PixelData, element); //获取图像数据  
	if (result.bad() || element == NULL)
		qDebug() << "file Load error" << endl;
	// 注释部分，8位dcm图像的读取
	//const Uint8* image_data = nullptr;
	//dataset->findAndGetUint8Array(DCM_PixelData, image_data);
	const Uint16* image16_data = nullptr;
	dataset->findAndGetUint16Array(DCM_PixelData, image16_data);
	if (image16_data != NULL)
	{
		if (1 == img_bits)
		{
			//cv::Mat dst(m_height, m_width, CV_8UC1, cv::Scalar::all(0));
			//unsigned char* data = nullptr;
			//for (int i = 0; i < m_height; i++)
			//{
			//	data = dst.ptr<unsigned char>(i); //取得每一行的头指针
			//	for (int j = 0; j < m_width; j++)
			//	{
			//		*data++ = image_data[i*m_width + j];
			//	}
			//}
			//16位灰度图
			cv::Mat dst = cv::Mat(m_height, m_width, CV_16UC1, cv::Scalar::all(0));
			unsigned short* data = nullptr;
			for (int i = 0; i < m_height; i++)
			{
				for (int j = 0; j < m_width; j++)
				{
					dst.at<unsigned short>(i, j) = image16_data[i*m_width + j];
				}
			}
			qDebug() << "dst1:" << dst.cols << 'x' << dst.rows << endl;
			return dst;
		}
		else if (3 == img_bits)
		{
			//cv::Mat dst(m_height, m_width, CV_8UC3, cv::Scalar::all(0));
			//unsigned short* data = nullptr;
			//for (int i = 0; i < m_height; i++)
			//{
			//	for (int j = 0; j < m_width; j++)
			//	{
			//		dst.at<cv::Vec3b>(i, j)[0] = image_data[i*m_width * 3 + j * 3 + 2];   //B  
			//		dst.at<cv::Vec3b>(i, j)[1] = image_data[i*m_width * 3 + j * 3 + 1];   //G 
			//		dst.at<cv::Vec3b>(i, j)[2] = image_data[i*m_width * 3 + j * 3];       //R 
			//	}
			//}
			//16位3通道图
			cv::Mat dst(m_height, m_width, CV_16UC3, cv::Scalar::all(0));
			unsigned short* data = nullptr;
			for (int i = 0; i < m_height; i++)
			{
				for (int j = 0; j < m_width; j++)
				{
					dst.at<cv::Vec3b>(i, j)[0] = image16_data[i*m_width * 3 + j * 3 + 2];   //B 
					dst.at<cv::Vec3b>(i, j)[1] = image16_data[i*m_width * 3 + j * 3 + 1];   //G  
					dst.at<cv::Vec3b>(i, j)[2] = image16_data[i*m_width * 3 + j * 3];       //R 
				}
			}
			qDebug() << "dst2:" << dst.cols << 'x' << dst.rows << endl;
			return dst;
		}
	}
}


vector<string> DcmProcess::QStrToStr(vector<QString> qstrs) {
	vector<string> strs;
	for (auto t : qstrs)
	{	
		//第一种方式会出现中文乱码
		//strs.push_back(t.toStdString());
		strs.push_back(std::string((const char *)t.toLocal8Bit().constData()));

	}
	return strs;
}


void DcmProcess::registerTag(int length, UINT16 GroupNum, UINT16 ElementNum) {
	DcmDataDictionary &dict = dcmDataDict.wrlock();
	//注册相应的tag
	for (Uint16 i = 0; i < length; i++) {
		dict.addEntry(new DcmDictEntry(GroupNum, ElementNum + i, EVR_UT, NULL, 1, 1, "private", OFTrue, NULL));
	}
	dcmDataDict.wrunlock();
}

void DcmProcess::addTagValue(DcmItem &item, vector<QString> QValueArr, UINT16 GroupNum, UINT16 ElementNum) {
	vector<string> ValueArr = QStrToStr(QValueArr);
	
	//条目名称tag赋值
	for (Uint16 i = 0; i < ValueArr.size(); i++) {
		item.putAndInsertString(DcmTag(GroupNum, ElementNum + i), ValueArr[i].c_str());
		cout << "单条目tag赋值: " << ValueArr[i] << endl;
		cout << "(" << hex << GroupNum << "," << ElementNum + i << ")" << endl;
	}
}

void DcmProcess::registerTag(int length, UINT16 GroupNum1, UINT16 GroupNum2, UINT16 ElementNum1, UINT16 ElementNum2) {
	DcmDataDictionary &dict = dcmDataDict.wrlock();
	//为每一个条目的名称和值都注册相应的tag
	for (Uint16 i = 0; i < length; i++) {
		dict.addEntry(new DcmDictEntry(GroupNum1, ElementNum1 + i, EVR_UT, NULL, 1, 1, "private", OFTrue, NULL));
		dict.addEntry(new DcmDictEntry(GroupNum2, ElementNum2 + i, EVR_UT, NULL, 1, 1, "private", OFTrue, NULL));
	}

	/*dict.addEntry(new DcmDictEntry(PRIVATE_CREATOR_TAG, EVR_UT, "PrivateText", 1, 1, "private", OFTrue, NULL));
	dict.addEntry(new DcmDictEntry(PRIVATE_ProductClass_TAG, EVR_UT, "PrivateText", 1, 1, "private", OFTrue, NULL));
	dict.addEntry(new DcmDictEntry(PRIVATE_ProductName_TAG, EVR_UT, "PrivateText", 1, 1, "private", OFTrue, NULL));
	dict.addEntry(new DcmDictEntry(PRIVATE_ProductID_TAG, EVR_UT, "PrivateText", 1, 1, "private", OFTrue, NULL));*/

	dcmDataDict.wrunlock();
}

void DcmProcess::addTagValue(DcmItem &item, vector<QString> QKeyArr, vector<QString> QValueArr, UINT16 GroupNum1, UINT16 GroupNum2, UINT16 ElementNum1, UINT16 ElementNum2) {
	vector<string> KeyArr = QStrToStr(QKeyArr);
	vector<string> ValueArr = QStrToStr(QValueArr);


	//条目名称tag赋值
	for (Uint16 i = 0; i < KeyArr.size(); i++) {
		item.putAndInsertString(DcmTag(GroupNum1, ElementNum1 + i), KeyArr[i].c_str());
		cout << "tag赋值，条目名称: " << KeyArr[i] << endl;
		cout << "(" << hex << GroupNum1 << "," << ElementNum1 + i << ")" << endl;
	}

	//条目值tag赋值
	for (Uint16 i = 0; i < ValueArr.size(); i++) {
		item.putAndInsertString(DcmTag(GroupNum2, ElementNum2 + i), ValueArr[i].c_str());
		cout << "tag赋值，条目值：" << ValueArr[i] << endl;
		cout << "(" << hex << GroupNum2 << "," << ElementNum2 + i << ")" << endl;
	}
}

bool DcmProcess::writeMarkInforToDcmTag(DcmItem &item, vector<QString> QValueArr, UINT16 GroupNum, UINT16 ElementNum) {
	vector<string> ValueArr = QStrToStr(QValueArr);

	int length = ValueArr.size();
	registerTag(length, GroupNum, ElementNum);
	addTagValue(item, QValueArr, GroupNum, ElementNum);
	return true;
}

bool DcmProcess::writeWorkInforToDcmTag(DcmItem &item, vector<QString> QKeyArr, vector<QString> QValueArr, UINT16 GroupNum1, UINT16 ElementNum1, UINT16 GroupNum2, UINT16 ElementNum2) {
	//一些初始判定条件，防止出错
	if (QKeyArr.size() != QValueArr.size()) {
		cout << "条目key 和 条目value数组的长度不相等" << endl;
		return false;
	}
	if (QKeyArr.size() <= 0) {
		cout << "条目数组为空" << endl;
		return false;
	}
	//cout << "注册tag函数" << endl;
	registerTag(QKeyArr.size(), GroupNum1, GroupNum2, ElementNum1, ElementNum2);
	//cout << "赋值tag函数" << endl;
	addTagValue(item, QKeyArr, QValueArr, GroupNum1, GroupNum2, ElementNum1, ElementNum2);
	return true;
}

bool DcmProcess::SaveImgAsDcm(cv::Mat img16Bit, QString QoutputPath, vector<QString> QworkInforKeyArr, vector<QString> QworkInforValueArr, vector<QString> QimagingKeyArr, vector<QString> QimagingValueArr, vector<QString> QmarkInforValueArr) {
	
	//string outputPath = QoutputPath.toStdString();
	string outputPath = std::string((const char*)QoutputPath.toLocal8Bit().constData());
	if (!outputPath.length()) {
		return false;
	}
	//预先读入dcm图像，空的dcm图像也可。才能保证保存一个新的dcm文件时不出错。
	//string filename = "./originalDcmImage.dcm";
	string filename = "./1.dcm";

	DcmFileFormat *fileFormat = new DcmFileFormat();
	fileFormat->loadFile(filename.c_str());
	DcmDataset *dataSet = fileFormat->getDataset();

	//初始化，需要添加到dcmTAG中的图像基本信息
	//所有dcm中的图像基本信息都是相同的
	int imgRow = img16Bit.rows;
	int imgCol = img16Bit.cols;
	OFString ImageType = "ORIGINAL";
	OFString Modality = "OT";
	OFString SeriesNumber = "1";
	OFString InstanceNumber = "1";
	unsigned short SamplesPerPixel = 1;
	OFString PhotometricInterpretation = "MONOCHROME2";
	int PlanarConfiguration = 0;
	unsigned short BitsAllocated = 16;
	unsigned short BitsStored = 16;
	unsigned short HighBit = 16;
	unsigned short PixelRepresentation = 0;
	float RescaleIntercept = 0;
	float RescaleSlope = 1;

	int length = imgRow * imgCol;
	ushort* img_pixel = new ushort[length];
	/*for (int i = 0; i < imgRow; i++) {
		for (int j = 0; j < imgCol; j++) {
			img_pixel[i*imgCol + j] = img16Bit.at<ushort>(i, j);
		}
	}*/
	ushort *Matdata = nullptr;
	for (int i = 0; i < imgRow; i++) {
		Matdata = img16Bit.ptr<ushort>(i);
		for (int j = 0; j < imgCol; j++) {
			img_pixel[i*imgCol + j] = Matdata[j];
		}
	}

	dataSet->putAndInsertOFStringArray(DCM_ImageType, ImageType);
	dataSet->putAndInsertOFStringArray(DCM_Modality, Modality);
	dataSet->putAndInsertOFStringArray(DCM_SeriesNumber, SeriesNumber);
	dataSet->putAndInsertOFStringArray(DCM_InstanceNumber, InstanceNumber);
	dataSet->putAndInsertUint16(DCM_SamplesPerPixel, SamplesPerPixel);
	dataSet->putAndInsertOFStringArray(DCM_PhotometricInterpretation, PhotometricInterpretation);
	dataSet->putAndInsertUint16(DCM_PlanarConfiguration, PlanarConfiguration);
	dataSet->putAndInsertUint16(DCM_Rows, imgRow);
	dataSet->putAndInsertUint16(DCM_Columns, imgCol);
	dataSet->putAndInsertUint16(DCM_BitsAllocated, BitsAllocated);
	dataSet->putAndInsertUint16(DCM_BitsStored, BitsStored);
	dataSet->putAndInsertUint16(DCM_HighBit, HighBit);
	dataSet->putAndInsertUint16(DCM_PixelRepresentation, PixelRepresentation);
	dataSet->putAndInsertFloat64(DCM_RescaleIntercept, RescaleIntercept);
	dataSet->putAndInsertFloat64(DCM_RescaleSlope, RescaleSlope);

	dataSet->putAndInsertUint16Array(DCM_PixelData, img_pixel, length);

	//保存成像信息到dcmTag
	UINT16 GroupNum1 = 0x6660;
	UINT16 GroupNum2 = 0x6661;
	UINT16 ElementNum1 = 0x0001;
	UINT16 ElementNum2 = 0x0001;
	//cout << "注册tag函数" << endl;
	registerTag(QimagingKeyArr.size(), GroupNum1, GroupNum2, ElementNum1, ElementNum2);
	//cout << "赋值tag函数" << endl;
	addTagValue(*fileFormat->getDataset(), QimagingKeyArr, QimagingValueArr, GroupNum1, GroupNum2, ElementNum1, ElementNum2);

	//保存工作信息到dcmTag
	UINT16 GroupNum3 = 0x6670;
	UINT16 GroupNum4 = 0x6671;
	UINT16 ElementNum3 = 0x0001;
	UINT16 ElementNum4 = 0x0001;
	writeWorkInforToDcmTag(*dataSet, QworkInforKeyArr, QworkInforValueArr, GroupNum3, ElementNum3, GroupNum4, ElementNum4);

	//保存标注信息到dcmTag
	UINT16 GroupNum5 = 0x6691;
	UINT16 ElementNum5 = 0x0001;
	writeMarkInforToDcmTag(*dataSet, QmarkInforValueArr, GroupNum5, ElementNum5);

	OFCondition status = fileFormat->saveFile(outputPath.c_str());
	if (status.bad()) {
		cout << "Error: cannot write DICOM file (" << status.text() << ")" << endl;
		return false;
	}
	delete fileFormat;
	delete[] img_pixel;
	return true;
}

void DcmProcess::getImgBaseInfor(QString dcmPath, vector<QString> &imgBaseInforKeyArr, vector<QString> &imgBaseInforValueArr) {
	if (!dcmPath.length())
	{
		cout << "dcmPath error!" << endl;
		return;
	}
	//先清空列表
	std::vector<QString>().swap(imgBaseInforKeyArr);
	std::vector<QString>().swap(imgBaseInforValueArr);

	DcmFileFormat fileformat;
	//读取Dicom图像
	OFCondition oc = fileformat.loadFile(std::string((const char*)dcmPath.toLocal8Bit().constData()).c_str());
	//判断Dicom文件是否读取成功 
	if (!oc.good())
	{
		cout << "file Load error" << std::endl;
		return;
	}
	DcmDataset *dataset = fileformat.getDataset();
	//给图像基本信息条目名称数组赋值
	imgBaseInforKeyArr.push_back("ImageType");
	imgBaseInforKeyArr.push_back("Modality");
	imgBaseInforKeyArr.push_back("SeriesNumber");
	imgBaseInforKeyArr.push_back("InstanceNumber");
	imgBaseInforKeyArr.push_back("SamplesPerPixel");
	imgBaseInforKeyArr.push_back("PhotometricInterpretation");
	imgBaseInforKeyArr.push_back("PlanarConfiguration");
	imgBaseInforKeyArr.push_back("Rows");
	imgBaseInforKeyArr.push_back("Columns");
	imgBaseInforKeyArr.push_back("BitsAllocated");
	imgBaseInforKeyArr.push_back("BitsStored");
	imgBaseInforKeyArr.push_back("HighBit");
	imgBaseInforKeyArr.push_back("PixelRepresentation");
	imgBaseInforKeyArr.push_back("RescaleIntercept");
	imgBaseInforKeyArr.push_back("RescaleSlope");
	imgBaseInforKeyArr.push_back("PixelData");

	vector<OFString> tempArr(16);
	dataset->findAndGetOFString(DCM_ImageType, tempArr[0]);
	dataset->findAndGetOFString(DCM_Modality, tempArr[1]);
	dataset->findAndGetOFString(DCM_SeriesNumber, tempArr[2]);
	dataset->findAndGetOFString(DCM_InstanceNumber, tempArr[3]);
	dataset->findAndGetOFString(DCM_SamplesPerPixel, tempArr[4]);
	dataset->findAndGetOFString(DCM_PhotometricInterpretation, tempArr[5]);
	dataset->findAndGetOFString(DCM_PlanarConfiguration, tempArr[6]);
	dataset->findAndGetOFString(DCM_Rows, tempArr[7]);
	dataset->findAndGetOFString(DCM_Columns, tempArr[8]);
	dataset->findAndGetOFString(DCM_BitsAllocated, tempArr[9]);
	dataset->findAndGetOFString(DCM_BitsStored, tempArr[10]);
	dataset->findAndGetOFString(DCM_HighBit, tempArr[11]);
	dataset->findAndGetOFString(DCM_PixelRepresentation, tempArr[12]);
	dataset->findAndGetOFString(DCM_RescaleIntercept, tempArr[13]);
	dataset->findAndGetOFString(DCM_RescaleSlope, tempArr[14]);
	dataset->findAndGetOFString(DCM_PixelData, tempArr[15]);

	//给图像基本信息条目值数组赋值
	for (int i = 0; i < tempArr.size(); i++) {
		string temp = tempArr[i].c_str();
		imgBaseInforValueArr.push_back(QString::fromStdString(temp));
	}
	return;
}
void DcmProcess::getImagingInfor(QString dcmPath, vector<QString> &imagingInforKeyArr, vector<QString> &imagingInforValueArr) {
	if (!dcmPath.length())
	{
		cout << "dcmPath error!" << endl;
		return;
	}
	//先清空列表
	std::vector<QString>().swap(imagingInforKeyArr);
	std::vector<QString>().swap(imagingInforValueArr);

	DcmFileFormat fileformat;
	//读取Dicom图像
	OFCondition oc = fileformat.loadFile(std::string((const char*)dcmPath.toLocal8Bit().constData()).c_str());
	//判断Dicom文件是否读取成功 
	if (!oc.good())
	{
		cout << "file Load error" << std::endl;
		return;
	}
	DcmDataset *dataset = fileformat.getDataset();

	UINT16 GroupNum1 = 0x6660;
	UINT16 GroupNum2 = 0x6661;
	UINT16 ElementNum1 = 0x0001;
	UINT16 ElementNum2 = 0x0001;

	//存储丢失的元素号，中间会有一个元素号对应的位置没有保存信息
	//用来辅助处理信息储存在不连续的元素号中
	vector<UINT16> MissElementNumArr;
	//首先对条目名称数组赋值
	for (int i = 0; ; i++) {
		//qDebug() << "ElementNum1 is:" << ElementNum1;
		DcmTag dcmtag(GroupNum1, ElementNum1);
		DcmTag dcmtag2(GroupNum1, ElementNum1 + 1);
		OFString tempKey;
		OFString tempKey2;
		dataset->findAndGetOFString(dcmtag, tempKey);
		dataset->findAndGetOFString(dcmtag2, tempKey2);
		//如果相邻两个位置都没有元素，就默认遍历完成，跳出循环
		if (tempKey.size() == 0 && tempKey2.size() == 0) {
			break;
		}
		else if (tempKey2.size() == 0) {
			imagingInforKeyArr.push_back(QString::fromLocal8Bit(tempKey.c_str()));
		}
		else if (tempKey.size() == 0) {
			MissElementNumArr.push_back(ElementNum1);
		}
		else {
			imagingInforKeyArr.push_back(QString::fromLocal8Bit(tempKey.c_str()));
		}
		ElementNum1++;
	}
	for (UINT16 i = ElementNum2; i < ElementNum1; i++) {
		int flag = 0;
		for (int j = 0; j < MissElementNumArr.size(); j++) {
			if (i == MissElementNumArr[j]) {
				flag = 1;
				break;
			}
		}
		//如果i不在miss数组中，正常添加；如果在，则不进行任何操作
		if (flag == 0) {
			DcmTag dcmtag(GroupNum2, i);
			OFString tempKey;
			dataset->findAndGetOFString(dcmtag, tempKey);
			imagingInforValueArr.push_back(QString::fromLocal8Bit(tempKey.c_str()));
		}
		else {

		}

	}

}

void DcmProcess::getWorkInfor(QString dcmPath, vector<QString> &workInforKeyArr, vector<QString> &workInforValueArr) {
	if (!dcmPath.length())
	{
		cout << "dcmPath error!" << endl;
		return;
	}
	//先清空列表
	std::vector<QString>().swap(workInforKeyArr);
	std::vector<QString>().swap(workInforValueArr);

	DcmFileFormat fileformat;
	//读取Dicom图像
	OFCondition oc = fileformat.loadFile(std::string((const char*)dcmPath.toLocal8Bit().constData()).c_str());
	//判断Dicom文件是否读取成功 
	if (!oc.good())
	{
		cout << "file Load error" << std::endl;
		return;
	}
	DcmDataset *dataset = fileformat.getDataset();

	UINT16 GroupNum1 = 0x6670;
	UINT16 GroupNum2 = 0x6671;
	UINT16 ElementNum1 = 0x0001;
	UINT16 ElementNum2 = 0x0001;

	//存储丢失的元素号，中间会有一个元素号对应的位置没有保存信息
	vector<UINT16> MissElementNumArr;
	//首先对条目名称数组赋值
	for (int i = 0; ; i++) {

		DcmTag dcmtag(GroupNum1, ElementNum1);
		DcmTag dcmtag2(GroupNum1, ElementNum1 + 1);
		OFString tempKey;
		OFString tempKey2;
		dataset->findAndGetOFString(dcmtag, tempKey);
		dataset->findAndGetOFString(dcmtag2, tempKey2);
		//qDebug() << "ElementNum1 is:" << ElementNum1 << " : " <<tempKey.size() ;
		//如果相邻两个位置都没有元素，就默认遍历完成，跳出循环
		if (tempKey.size() == 0 && tempKey2.size() == 0) {
			break;
		}
		else if (tempKey2.size() == 0) {
			workInforKeyArr.push_back(QString::fromLocal8Bit(tempKey.c_str()));
		}
		else if (tempKey.size() == 0) {
			MissElementNumArr.push_back(ElementNum1);
		}
		else {
			workInforKeyArr.push_back(QString::fromLocal8Bit(tempKey.c_str()));
		}
		ElementNum1++;
	}
	for (UINT16 i = ElementNum2; i < ElementNum1; i++) {
		int flag = 0;
		for (int j = 0; j < MissElementNumArr.size(); j++) {
			if (i == MissElementNumArr[j]) {
				flag = 1;
				break;
			}
		}
		//如果i不在miss数组中，正常添加；如果在，则不进行任何操作
		if (flag == 0) {
			DcmTag dcmtag(GroupNum2, i);
			OFString tempKey;
			dataset->findAndGetOFString(dcmtag, tempKey);
			workInforValueArr.push_back(QString::fromLocal8Bit(tempKey.c_str()));
		}
		else {

		}

	}
}
void DcmProcess::getMarkInfor(QString dcmPath, vector<QString> &markInforArr) {
	if (!dcmPath.length())
	{
		cout << "dcmPath error!" << endl;
		return;
	}
	//先清空列表
	std::vector<QString>().swap(markInforArr);

	DcmFileFormat fileformat;
	//读取Dicom图像
	OFCondition oc = fileformat.loadFile(std::string((const char*)dcmPath.toLocal8Bit().constData()).c_str());
	//判断Dicom文件是否读取成功 
	if (!oc.good())
	{
		cout << "file Load error" << std::endl;
		return;
	}
	DcmDataset *dataset = fileformat.getDataset();

	UINT16 GroupNum = 0x6691;
	UINT16 ElementNum = 0x0001;
	//首先对条目名称数组赋值
	for (int i = 0; ; i++) {

		DcmTag dcmtag(GroupNum, ElementNum);
		DcmTag dcmtag2(GroupNum, ElementNum + 1);
		OFString tempKey;
		OFString tempKey2;
		dataset->findAndGetOFString(dcmtag, tempKey);
		dataset->findAndGetOFString(dcmtag2, tempKey2);
		//qDebug() << "ElementNum1 is:" << ElementNum1 << " : " <<tempKey.size() ;
		//如果相邻两个位置都没有元素，就默认遍历完成，跳出循环
		//如果只是一个位置没有元素，还会继续向后遍历
		if (tempKey.size() == 0 && tempKey2.size() == 0) {
			break;
		}
		else if (tempKey2.size() == 0) {
			markInforArr.push_back(QString::fromLocal8Bit(tempKey.c_str()));
		}
		else if (tempKey.size() == 0) {

		}
		else {
			markInforArr.push_back(QString::fromLocal8Bit(tempKey.c_str()));
		}
		ElementNum++;
	}

}


QString DcmProcess::FourIntToMarkString(int type, int x, int y, int w, int h, QString text) {
	QString totalStr = "";
	totalStr = "M" + QString::number(type) + "X" + QString::number(x) + "Y" + QString::number(y) + "W" + QString::number(w) + "H" + QString::number(h) + "T" + text;
	return totalStr;
}
QString DcmProcess::MarkStrToFourInt(QString originalStr, int &type, int &x, int &y, int &w, int &h) {
	QString text = "";
	for (int i = 0; i < originalStr.size(); i++) {
		if (originalStr.at(i) == 'M') {
			QString intStr = "";
			i++;
			while (originalStr.at(i) >= '0'&&originalStr.at(i) <= '9') {
				intStr.append(originalStr.at(i));
				i++;
			}
			type = intStr.toInt();
			i--;
		}
		else if (originalStr.at(i) == 'X') {
			QString intStr = "";
			i++;
			while (originalStr.at(i) >= '0'&&originalStr.at(i) <= '9') {
				intStr.append(originalStr.at(i));
				i++;
			}
			x = intStr.toInt();
			i--;
		}
		else if (originalStr.at(i) == 'Y') {
			QString intStr = "";
			i++;
			while (originalStr.at(i) >= '0'&&originalStr.at(i) <= '9') {
				intStr.append(originalStr.at(i));
				i++;
			}
			y = intStr.toInt();
			i--;
		}
		else if (originalStr.at(i) == 'W') {
			QString intStr = "";
			i++;
			while (originalStr.at(i) >= '0'&&originalStr.at(i) <= '9') {
				intStr.append(originalStr.at(i));
				i++;
			}
			w = intStr.toInt();
			i--;
		}
		else if (originalStr.at(i) == 'H') {
			QString intStr = "";
			i++;
			while (originalStr.at(i) >= '0'&&originalStr.at(i) <= '9') {
				intStr.append(originalStr.at(i));
				i++;
			}
			h = intStr.toInt();
			i--;
		}
		else if (originalStr.at(i) == 'T') {
			//如果T的位置不在字符串末尾，则说明后面还有文字部分；否则直接返回空串
			if (i < originalStr.size()) {
				i++;
				text = originalStr.mid(i);
				break;
			}
		}
	}

	return text;
}


void DcmProcess::PrintAllTag(QString dcmfilepath) {

	if (!dcmfilepath.length())
	{
		qDebug() << "dcmfilepath error!";
		return;
	}

	DcmFileFormat fileformat;
	//读取Dicom图像
	OFCondition oc = fileformat.loadFile(dcmfilepath.toStdString().c_str());
	//判断Dicom文件是否读取成功 
	if (!oc.good())
	{
		qDebug() << "file Load error";
		return;
	}
	DcmMetaInfo *MetaInfo = fileformat.getMetaInfo();
	DcmDataset *data = fileformat.getDataset();
	DcmObject *item = data->nextInContainer(NULL);
	int i = 0;
	/*item = MetaInfo->nextInContainer(NULL);*/
	while (item) {
		DcmTag dcmtag = item->getTag();
		const char* tagName = dcmtag.getTagName();
		const char* tagVR = dcmtag.getVRName();
		const char* privateName = dcmtag.getPrivateCreator();
		cout << "tag name is:" << tagName << "\t\t";
		cout << "tag VR is:" << tagVR << "\t\t";
		if (privateName != NULL)
			cout << "PrivateCreator is:" << privateName << "\t\t";
		cout << "tag number is :" << item->getTag().toString().data() << "\t\t";
		OFString itemValue;
		data->findAndGetOFString(dcmtag, itemValue);
		cout << "itemValue is:" << itemValue.c_str() << endl;
		//qDebug() << "itemValue is:" << QString::fromLocal8Bit(itemValue.c_str());

		item = data->nextInContainer(item);
		i++;
	}
	cout << "all tags num is " << i << endl;
	return;

}

