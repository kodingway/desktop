#include "logindialog.h"
#include "pmsettings.h"
#include "webservice.h"

#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QtGui>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent,bool canautologin, QString *settingsFilename) : LoginDialogBase(parent)
{
	this->settingsFilename = *settingsFilename;
	PMSettings *pmsettings = new PMSettings(this->settingsFilename);

	ui.PasswordEdit->setText(pmsettings->getAttributeSettings("password"));
	ui.HostEdit->setText(pmsettings->getAttributeSettings("host"));
	ui.UsernameEdit->setText(pmsettings->getAttributeSettings("username"));

    if (pmsettings->getAttributeSettings("remember") == "true"){
        ui.RememberLoginsChk->setChecked(true);
    } else {
        ui.RememberLoginsChk->setChecked(false);
    }

	if (pmsettings->getAttributeSettings("autologin") == "true")
    {
        ui.AutoLogincheckBox->setChecked(true);
        delete pmsettings;

        if (canautologin == true) canContinue();

    } else {
        delete pmsettings;
    }
}

LoginDialog::~LoginDialog()
{

}

void LoginDialog::on_cancelButton_clicked()
{
    QDialog::reject();
}

void LoginDialog::canContinue()
{
		PMSettings *pmsettings = new PMSettings(settingsFilename);
        if (pmsettings->getAttributeSettings("autologin") == "true")
        {
            LhcWebServiceClient *lhwsc = LhcWebServiceClient::instance();
            QString host = ui.HostEdit->text().replace(QString("index.php"), QString(""));

            bool mode;

            if (host.indexOf("https://") != -1){
                mode = true;
                host = host.replace(QString("https://"),QString(""));
            } else {
                mode = false;
                host = host.replace(QString("http://"),QString(""));
            }

            if (!host.endsWith("/")){
                host += "/";
            }

            lhwsc->setFetchURL(host,mode);
            delete pmsettings;

            QStringList filter;
            filter.append("username="+QUrl::toPercentEncoding(ui.UsernameEdit->text()));
            filter.append("password="+QUrl::toPercentEncoding(ui.PasswordEdit->text()));

            lhwsc->setLogins(ui.UsernameEdit->text(),ui.PasswordEdit->text());
            lhwsc->LhcSendRequestAuthorization(filter,"/xml/checklogin/",(QObject*) this, LoginDialog::LoginCheckedCallback);
        }

}

void LoginDialog::LoginCheckedCallback(void* pt2Object, QByteArray result)
{
    LoginDialog* mySelf = (LoginDialog*) pt2Object;

    QScriptValue sc;
    QScriptEngine engine;
    sc = engine.evaluate("(" +QString(result)+ ")");

    if (sc.property("result").toBoolean() == true)
    {
        mySelf->accept();
    } else {
        QMessageBox::warning(NULL, tr("Authentication failed"),
									 tr("Authentication failed"),
									 tr("&OK"), QString::null , 0, 0, 1);
    }
}

void LoginDialog::on_okButton_clicked()
{

	lgUserName = ui.UsernameEdit->text();

	if (!lgUserName.isEmpty())
	{

			lgUserPassword = ui.PasswordEdit->text();

			if (!lgUserPassword.isEmpty())
			{
                QString host = ui.HostEdit->text().replace(QString("index.php"), QString(""));
                bool mode;

                if (host.indexOf("https://") != -1){
                    mode = true;
                    host = host.replace(QString("https://"),QString(""));
                } else {
                    mode = false;
                    host = host.replace(QString("http://"),QString(""));
                }

                if (!host.endsWith("/")){
                    host += "/";
                }

                if (!host.isEmpty())
				{
						PMSettings *pmsettings = new PMSettings(settingsFilename);

                        if (ui.RememberLoginsChk->isChecked()){
                            pmsettings->setAttribute("username",lgUserName);
                            pmsettings->setAttribute("password",lgUserPassword);
                            pmsettings->setAttribute("remember","true");
                        } else {
                            pmsettings->setAttribute("username","");
                            pmsettings->setAttribute("password","");
                            pmsettings->setAttribute("remember","false");
                        }

                        pmsettings->setAttribute("host", (mode == true ? "https://" : "http://")+host);

                        if (ui.AutoLogincheckBox->isChecked())
							pmsettings->setAttribute("autologin","true");
						else
							pmsettings->setAttribute("autologin","false");


						pmsettings->sync();



						delete pmsettings;

                        LhcWebServiceClient *lhwsc = LhcWebServiceClient::instance();
                        lhwsc->setFetchURL(host,mode);

                        QStringList filter;
                        filter.append("username="+QUrl::toPercentEncoding(lgUserName));
                        filter.append("password="+QUrl::toPercentEncoding(lgUserPassword));

                        lhwsc->setLogins(lgUserName,lgUserPassword);
                        lhwsc->LhcSendRequestAuthorization(filter,"/xml/checklogin/",(QObject*) this, LoginDialog::LoginCheckedCallback);

				}else {
					QMessageBox::warning(this, tr("Warning"),
								 tr("The host field is empty!"),
								 tr("&OK"), QString::null , 0, 0, 1);
					ui.HostEdit->setFocus();
				}

			}else {
                QMessageBox::warning(this, tr("Warning"),
								 tr("The password field is empty!"),
								 tr("&OK"), QString::null , 0, 0, 1);
				ui.PasswordEdit->setFocus();
			}


    } else {
		QMessageBox::warning(this, tr("Warning"),
                             tr("The username field is empty!"),
                             tr("&OK"), QString::null , 0, 0, 1);
        ui.UsernameEdit->setFocus();
	}
}
