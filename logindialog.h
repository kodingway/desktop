#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "logindialogbase.h"


class LoginDialog : public LoginDialogBase
{
	Q_OBJECT
		
	public:
		LoginDialog(QWidget *parent = 0,bool canautologin = false, QString *settingsFilename = NULL);
		~LoginDialog();

    static void LoginCheckedCallback(void* pt2Object, QByteArray result);
    void canContinue();

	private slots:
		virtual void on_okButton_clicked();
		virtual void on_cancelButton_clicked();
        

	private:
    QString lgUserName,       ///< Vartotojo tapatybės vardas. 
            lgUserPassword,   ///< Vartotojo tapatybės slaptažodis.
			lgHost,           ///< Serverio adresas.
			settingsFilename;

           
};

#endif
