#include "sscardreport.h"
#include "ui_sscardreport.h"
#include "Gloabal.h"

SSCardReport::SSCardReport(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::SSCardReport)
{
	ui->setupUi(this);

}

SSCardReport::~SSCardReport()
{
	delete ui;
}

int SSCardReport::SearchCardProgres()
{
	QString strAppPath = qApp->applicationDirPath();
	QString path = strAppPath + "/data";
	QDirIterator iter(path, QStringList() << "Person*.json", QDir::Files | QDir::NoSymLinks);
	QStringList strNameList;
	QDate dtToday = QDate::currentDate();
	qDebug() << "Today:" << dtToday.toString();
	while (iter.hasNext())
	{
		iter.next();
		//qDebug() << "fileName:" << iter.fileName();  // 只有文件名
		//qDebug() << "filePath:" << iter.filePath();  //包含文件名的文件的全路径
		QDateTime dtFile = iter.fileInfo().lastModified();
		qDebug() << iter.filePath() << "\t" << dtFile.date().toString();
		if (dtFile.date() == dtToday)
		{
			vecCardProgress.emplace_back(iter.filePath());
		}
	}
	return vecCardProgress.size();
}

bool SSCardReport::GetSSCardInfo(CJsonObject&& json, string& strSSCard, string& strBankCard)
{
	if (!json.KeyExist("saveCardJrzhActive"))		// 必须要有最终激活步骤
		return false;

	if (!json.KeyExist("WriteCard"))				// 获取银行卡号
		return false;
	CJsonObject jsonWrite = json["WriteCard"];
	if (jsonWrite.KeyExist("datagram"))
	{
		CJsonObject jsonDatagram = jsonWrite["datagram"];
		if (!jsonDatagram.KeyExist("BankNum"))
			return false;
		if (!jsonDatagram.Get("BankNum", strBankCard))
			return false;
		return true;
	}

	if (!json.KeyExist("CardNum"))					// 获取社保卡号
		return false;
	if (json.Get("CardNum", strSSCard))
		return true;
	else
		return false;
}

int SSCardReport::LoadCardProgress()
{
	if (!vecCardProgress.size())
		return 0;
	int nCount = 0;
	for (auto var : vecCardProgress)
	{
		try
		{
			ifstream ifs(var.toStdString());
			if (!ifs.is_open())
				continue;

			stringstream ss;
			ss << ifs.rdbuf();
			CJsonObject JsonProgress(ss.str());
			if (!JsonProgress.KeyExist("Person"))
				continue;

			CJsonObject jsonPerson = JsonProgress["Person"];
			if (jsonPerson.IsEmpty())
				continue;
			string strName, strID, strSSCard, strBankCard, strGender, strMobile, strServiceType, strNation;
			if (!jsonPerson.Get("Name", strName))
				continue;
			if (!jsonPerson.Get("Identity", strID))
				continue;
			jsonPerson.Get("Gender", strGender);
			jsonPerson.Get("Nation", strNation);
			jsonPerson.Get("Mobile", strMobile);
			if (JsonProgress.KeyExist("NewCard"))
			{
				strServiceType = "新办";
				if (!GetSSCardInfo(std::move(JsonProgress["NewCard"]), strSSCard, strBankCard))
					continue;
			}
			else if (JsonProgress.KeyExist("UpdateCard"))
			{
				strServiceType = "补办";
				if (!GetSSCardInfo(std::move(JsonProgress["UpdateCard"]), strSSCard, strBankCard))
					continue;
			}
			else
				continue;

			// 已经完成数据解析，接下来做把数据填入表格,并生成xlxs文件和发送邮件 2022/07/01
			nCount++;
		}
		catch (std::exception& e)
		{
			gInfo() << "Catch a exception:" << e.what();
		}
	}
}


void SSCardReport::on_pushButton_Send_clicked()
{

}
