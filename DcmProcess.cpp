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

	//��ô����﷨�ַ��� 
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
		//��ѹ����ͼ�����ؽ��н�ѹ
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

	//����dataset����DicomImage����Ҫ����Ľ�ѹ������ 
	DicomImage* m_dcmImage = new DicomImage((DcmObject*)dataset, xfer);

	return m_dcmImage;

}

vector<cv::Mat> DcmProcess::GetImageFromDcmDataSet(DicomImage *m_dcmImage) {
	//���ͼ������
	vector<cv::Mat> output_img;
	//��ȡ����ļ�������ͼ���֡��
	int framecount(m_dcmImage->getFrameCount());
	cout << "Dcm�ļ���һ���м�֡ͼ��framecount is:" << framecount << endl;
	for (int k = 0; k < framecount; k++)
	{
		unsigned short *pixelData = (unsigned short*)(m_dcmImage->getOutputData(16, k, 0)); //���16λ��ͼ������ָ��  
		if (pixelData != NULL)
		{
			int m_height = m_dcmImage->getHeight();
			int m_width = m_dcmImage->getWidth();
			// 1��ʾ�Ҷ�ͼ��3��ʾ��ɫͼ��
			// �����趨Ĭ��Ϊ16Ϊ�Ҷ�ͼ��
			this->Img_bitCount = 1;
			if (3 == this->Img_bitCount)
			{
				cout << "ͨ����Ϊ3" << endl;
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
				cout << "ͨ����Ϊ1" << endl;
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
	//��ȡDicomͼ��
	OFCondition oc = fileformat.loadFile(filepath.toStdString().c_str());
	//�ж�Dicom�ļ��Ƿ��ȡ�ɹ� 
	if (!oc.good())
	{
		std::cout << "file Load error" << std::endl;
		return false;
	}
	DcmDataset *dataset = fileformat.getDataset();                              //�õ�Dicom�����ݼ�  
	E_TransferSyntax xfer = dataset->getOriginalXfer();                          //�õ������﷨  


	 //1��ʾ��ɫͼ��3��ʾ��ɫͼ��
	unsigned short img_bits(0);
	dataset->findAndGetUint16(DCM_SamplesPerPixel, img_bits);

	unsigned short m_width;                                                     //��ȡͼ��Ĵ����  
	unsigned short m_height;
	dataset->findAndGetUint16(DCM_Rows, m_height);
	dataset->findAndGetUint16(DCM_Columns, m_width);


	//��ô����﷨�ַ��� 
	const char* transferSyntax = NULL;
	fileformat.getMetaInfo()->findAndGetString(DCM_TransferSyntaxUID, transferSyntax);

	//�����Ӧ����Ϣ
	/*cout << "img_bits is:" << img_bits << endl;
	cout << "���߷ֱ��ǣ�" << m_width << " " << m_height << endl;
	cout << "�����﷨�ַ����ǣ�" << transferSyntax << endl;*/

	string losslessTransUID = "1.2.840.10008.1.2.4.70";
	string lossTransUID = "1.2.840.10008.1.2.4.51";
	string losslessP14 = "1.2.840.10008.1.2.4.57";
	string lossyP1 = "1.2.840.10008.1.2.4.50";
	string lossyRLE = "1.2.840.10008.1.2.5";
	if (transferSyntax == losslessTransUID || transferSyntax == lossTransUID ||
		transferSyntax == losslessP14 || transferSyntax == lossyP1)
	{
		DJDecoderRegistration::registerCodecs();
		dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);                       //��ѹ����ͼ�����ؽ��н�ѹ  
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
	OFCondition result = dataset->findAndGetElement(DCM_PixelData, element); //��ȡͼ������  
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
			//	data = dst.ptr<unsigned char>(i); //ȡ��ÿһ�е�ͷָ��
			//	for (int j = 0; j < m_width; j++)
			//	{
			//		*data++ = image_data[i*m_width + j];
			//	}
			//}
			//16λ�Ҷ�ͼ��ʾ
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
			//16λ3ͨ��ͼ��ʾ
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
	//��ȡDicomͼ��
	//��һ�ֽ�QStringת��const char,���Ļ�����޷������ļ�
	//OFCondition oc = fileformat.loadFile(filepath.toStdString().c_str());
	OFCondition oc = fileformat.loadFile(std::string((const char*)filepath.toLocal8Bit().constData()).c_str());
	//�ж�Dicom�ļ��Ƿ��ȡ�ɹ� 
	if (!oc.good())
	{
		qDebug() << "file Load error" << endl;
	}
	DcmDataset *dataset = fileformat.getDataset();                              //�õ�Dicom�����ݼ�  
	E_TransferSyntax xfer = dataset->getOriginalXfer();                          //�õ������﷨  


	 //1��ʾ��ɫͼ��3��ʾ��ɫͼ��
	unsigned short img_bits(0);
	dataset->findAndGetUint16(DCM_SamplesPerPixel, img_bits);

	unsigned short m_width;                                                     //��ȡͼ��Ĵ����  
	unsigned short m_height;
	dataset->findAndGetUint16(DCM_Rows, m_height);
	dataset->findAndGetUint16(DCM_Columns, m_width);


	//��ô����﷨�ַ��� 
	const char* transferSyntax = NULL;
	fileformat.getMetaInfo()->findAndGetString(DCM_TransferSyntaxUID, transferSyntax);

	//�����Ӧ����Ϣ
	/*cout << "img_bits is:" << img_bits << endl;
	cout << "���߷ֱ��ǣ�" << m_width << " " << m_height << endl;
	cout << "�����﷨�ַ����ǣ�" << transferSyntax << endl;*/

	string losslessTransUID = "1.2.840.10008.1.2.4.70";
	string lossTransUID = "1.2.840.10008.1.2.4.51";
	string losslessP14 = "1.2.840.10008.1.2.4.57";
	string lossyP1 = "1.2.840.10008.1.2.4.50";
	string lossyRLE = "1.2.840.10008.1.2.5";
	if (transferSyntax == losslessTransUID || transferSyntax == lossTransUID ||
		transferSyntax == losslessP14 || transferSyntax == lossyP1)
	{
		DJDecoderRegistration::registerCodecs();
		dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);                       //��ѹ����ͼ�����ؽ��н�ѹ  
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
	OFCondition result = dataset->findAndGetElement(DCM_PixelData, element); //��ȡͼ������  
	if (result.bad() || element == NULL)
		qDebug() << "file Load error" << endl;
	// ע�Ͳ��֣�8λdcmͼ��Ķ�ȡ
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
			//	data = dst.ptr<unsigned char>(i); //ȡ��ÿһ�е�ͷָ��
			//	for (int j = 0; j < m_width; j++)
			//	{
			//		*data++ = image_data[i*m_width + j];
			//	}
			//}
			//16λ�Ҷ�ͼ
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
			//16λ3ͨ��ͼ
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
		//��һ�ַ�ʽ�������������
		//strs.push_back(t.toStdString());
		strs.push_back(std::string((const char *)t.toLocal8Bit().constData()));

	}
	return strs;
}


void DcmProcess::registerTag(int length, UINT16 GroupNum, UINT16 ElementNum) {
	DcmDataDictionary &dict = dcmDataDict.wrlock();
	//ע����Ӧ��tag
	for (Uint16 i = 0; i < length; i++) {
		dict.addEntry(new DcmDictEntry(GroupNum, ElementNum + i, EVR_UT, NULL, 1, 1, "private", OFTrue, NULL));
	}
	dcmDataDict.wrunlock();
}

void DcmProcess::addTagValue(DcmItem &item, vector<QString> QValueArr, UINT16 GroupNum, UINT16 ElementNum) {
	vector<string> ValueArr = QStrToStr(QValueArr);
	
	//��Ŀ����tag��ֵ
	for (Uint16 i = 0; i < ValueArr.size(); i++) {
		item.putAndInsertString(DcmTag(GroupNum, ElementNum + i), ValueArr[i].c_str());
		cout << "����Ŀtag��ֵ: " << ValueArr[i] << endl;
		cout << "(" << hex << GroupNum << "," << ElementNum + i << ")" << endl;
	}
}

void DcmProcess::registerTag(int length, UINT16 GroupNum1, UINT16 GroupNum2, UINT16 ElementNum1, UINT16 ElementNum2) {
	DcmDataDictionary &dict = dcmDataDict.wrlock();
	//Ϊÿһ����Ŀ�����ƺ�ֵ��ע����Ӧ��tag
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


	//��Ŀ����tag��ֵ
	for (Uint16 i = 0; i < KeyArr.size(); i++) {
		item.putAndInsertString(DcmTag(GroupNum1, ElementNum1 + i), KeyArr[i].c_str());
		cout << "tag��ֵ����Ŀ����: " << KeyArr[i] << endl;
		cout << "(" << hex << GroupNum1 << "," << ElementNum1 + i << ")" << endl;
	}

	//��Ŀֵtag��ֵ
	for (Uint16 i = 0; i < ValueArr.size(); i++) {
		item.putAndInsertString(DcmTag(GroupNum2, ElementNum2 + i), ValueArr[i].c_str());
		cout << "tag��ֵ����Ŀֵ��" << ValueArr[i] << endl;
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
	//һЩ��ʼ�ж���������ֹ����
	if (QKeyArr.size() != QValueArr.size()) {
		cout << "��Ŀkey �� ��Ŀvalue����ĳ��Ȳ����" << endl;
		return false;
	}
	if (QKeyArr.size() <= 0) {
		cout << "��Ŀ����Ϊ��" << endl;
		return false;
	}
	//cout << "ע��tag����" << endl;
	registerTag(QKeyArr.size(), GroupNum1, GroupNum2, ElementNum1, ElementNum2);
	//cout << "��ֵtag����" << endl;
	addTagValue(item, QKeyArr, QValueArr, GroupNum1, GroupNum2, ElementNum1, ElementNum2);
	return true;
}

bool DcmProcess::SaveImgAsDcm(cv::Mat img16Bit, QString QoutputPath, vector<QString> QworkInforKeyArr, vector<QString> QworkInforValueArr, vector<QString> QimagingKeyArr, vector<QString> QimagingValueArr, vector<QString> QmarkInforValueArr) {
	
	//string outputPath = QoutputPath.toStdString();
	string outputPath = std::string((const char*)QoutputPath.toLocal8Bit().constData());
	if (!outputPath.length()) {
		return false;
	}
	//Ԥ�ȶ���dcmͼ�񣬿յ�dcmͼ��Ҳ�ɡ����ܱ�֤����һ���µ�dcm�ļ�ʱ������
	//string filename = "./originalDcmImage.dcm";
	string filename = "./1.dcm";

	DcmFileFormat *fileFormat = new DcmFileFormat();
	fileFormat->loadFile(filename.c_str());
	DcmDataset *dataSet = fileFormat->getDataset();

	//��ʼ������Ҫ��ӵ�dcmTAG�е�ͼ�������Ϣ
	//����dcm�е�ͼ�������Ϣ������ͬ��
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

	//���������Ϣ��dcmTag
	UINT16 GroupNum1 = 0x6660;
	UINT16 GroupNum2 = 0x6661;
	UINT16 ElementNum1 = 0x0001;
	UINT16 ElementNum2 = 0x0001;
	//cout << "ע��tag����" << endl;
	registerTag(QimagingKeyArr.size(), GroupNum1, GroupNum2, ElementNum1, ElementNum2);
	//cout << "��ֵtag����" << endl;
	addTagValue(*fileFormat->getDataset(), QimagingKeyArr, QimagingValueArr, GroupNum1, GroupNum2, ElementNum1, ElementNum2);

	//���湤����Ϣ��dcmTag
	UINT16 GroupNum3 = 0x6670;
	UINT16 GroupNum4 = 0x6671;
	UINT16 ElementNum3 = 0x0001;
	UINT16 ElementNum4 = 0x0001;
	writeWorkInforToDcmTag(*dataSet, QworkInforKeyArr, QworkInforValueArr, GroupNum3, ElementNum3, GroupNum4, ElementNum4);

	//�����ע��Ϣ��dcmTag
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
	//������б�
	std::vector<QString>().swap(imgBaseInforKeyArr);
	std::vector<QString>().swap(imgBaseInforValueArr);

	DcmFileFormat fileformat;
	//��ȡDicomͼ��
	OFCondition oc = fileformat.loadFile(std::string((const char*)dcmPath.toLocal8Bit().constData()).c_str());
	//�ж�Dicom�ļ��Ƿ��ȡ�ɹ� 
	if (!oc.good())
	{
		cout << "file Load error" << std::endl;
		return;
	}
	DcmDataset *dataset = fileformat.getDataset();
	//��ͼ�������Ϣ��Ŀ�������鸳ֵ
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

	//��ͼ�������Ϣ��Ŀֵ���鸳ֵ
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
	//������б�
	std::vector<QString>().swap(imagingInforKeyArr);
	std::vector<QString>().swap(imagingInforValueArr);

	DcmFileFormat fileformat;
	//��ȡDicomͼ��
	OFCondition oc = fileformat.loadFile(std::string((const char*)dcmPath.toLocal8Bit().constData()).c_str());
	//�ж�Dicom�ļ��Ƿ��ȡ�ɹ� 
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

	//�洢��ʧ��Ԫ�غţ��м����һ��Ԫ�غŶ�Ӧ��λ��û�б�����Ϣ
	//��������������Ϣ�����ڲ�������Ԫ�غ���
	vector<UINT16> MissElementNumArr;
	//���ȶ���Ŀ�������鸳ֵ
	for (int i = 0; ; i++) {
		//qDebug() << "ElementNum1 is:" << ElementNum1;
		DcmTag dcmtag(GroupNum1, ElementNum1);
		DcmTag dcmtag2(GroupNum1, ElementNum1 + 1);
		OFString tempKey;
		OFString tempKey2;
		dataset->findAndGetOFString(dcmtag, tempKey);
		dataset->findAndGetOFString(dcmtag2, tempKey2);
		//�����������λ�ö�û��Ԫ�أ���Ĭ�ϱ�����ɣ�����ѭ��
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
		//���i����miss�����У�������ӣ�����ڣ��򲻽����κβ���
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
	//������б�
	std::vector<QString>().swap(workInforKeyArr);
	std::vector<QString>().swap(workInforValueArr);

	DcmFileFormat fileformat;
	//��ȡDicomͼ��
	OFCondition oc = fileformat.loadFile(std::string((const char*)dcmPath.toLocal8Bit().constData()).c_str());
	//�ж�Dicom�ļ��Ƿ��ȡ�ɹ� 
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

	//�洢��ʧ��Ԫ�غţ��м����һ��Ԫ�غŶ�Ӧ��λ��û�б�����Ϣ
	vector<UINT16> MissElementNumArr;
	//���ȶ���Ŀ�������鸳ֵ
	for (int i = 0; ; i++) {

		DcmTag dcmtag(GroupNum1, ElementNum1);
		DcmTag dcmtag2(GroupNum1, ElementNum1 + 1);
		OFString tempKey;
		OFString tempKey2;
		dataset->findAndGetOFString(dcmtag, tempKey);
		dataset->findAndGetOFString(dcmtag2, tempKey2);
		//qDebug() << "ElementNum1 is:" << ElementNum1 << " : " <<tempKey.size() ;
		//�����������λ�ö�û��Ԫ�أ���Ĭ�ϱ�����ɣ�����ѭ��
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
		//���i����miss�����У�������ӣ�����ڣ��򲻽����κβ���
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
	//������б�
	std::vector<QString>().swap(markInforArr);

	DcmFileFormat fileformat;
	//��ȡDicomͼ��
	OFCondition oc = fileformat.loadFile(std::string((const char*)dcmPath.toLocal8Bit().constData()).c_str());
	//�ж�Dicom�ļ��Ƿ��ȡ�ɹ� 
	if (!oc.good())
	{
		cout << "file Load error" << std::endl;
		return;
	}
	DcmDataset *dataset = fileformat.getDataset();

	UINT16 GroupNum = 0x6691;
	UINT16 ElementNum = 0x0001;
	//���ȶ���Ŀ�������鸳ֵ
	for (int i = 0; ; i++) {

		DcmTag dcmtag(GroupNum, ElementNum);
		DcmTag dcmtag2(GroupNum, ElementNum + 1);
		OFString tempKey;
		OFString tempKey2;
		dataset->findAndGetOFString(dcmtag, tempKey);
		dataset->findAndGetOFString(dcmtag2, tempKey2);
		//qDebug() << "ElementNum1 is:" << ElementNum1 << " : " <<tempKey.size() ;
		//�����������λ�ö�û��Ԫ�أ���Ĭ�ϱ�����ɣ�����ѭ��
		//���ֻ��һ��λ��û��Ԫ�أ��������������
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
			//���T��λ�ò����ַ���ĩβ����˵�����滹�����ֲ��֣�����ֱ�ӷ��ؿմ�
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
	//��ȡDicomͼ��
	OFCondition oc = fileformat.loadFile(dcmfilepath.toStdString().c_str());
	//�ж�Dicom�ļ��Ƿ��ȡ�ɹ� 
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

