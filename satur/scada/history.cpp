#include "history.h"
#include "IoNetClient.h"
#include "ui_history.h"

#include <QDebug>



RHistorySelect::RHistorySelect(IoNetClient &src,struct trendinfo *tp,QWidget *p /*=NULL*/) :
        QDialog(p),
        s(src),
        TrendParam(tp),
        m_ui(new Ui::History)
{
    m_ui->setupUi(this);

    connect(m_ui->trend1_reg101,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend1_reg102,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend1_reg103,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend1_reg104,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend1_reg105,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend1_reg106,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend1_reg107,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend1_reg108,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend1_reg109,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend1_reg110,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend1_reg111,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend1_reg112,SIGNAL(clicked()),this,SLOT(slotAccept()));

    connect(m_ui->trend1_flow11,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend1_flow12,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend1_level11,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend1_presh11,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend1_ph11,SIGNAL(clicked()),this,SLOT(slotAccept()));


    connect(m_ui->trend2_reg201,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend2_reg202,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend2_reg203,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend2_reg204,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend2_reg205,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend2_reg206,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend2_reg207,SIGNAL(clicked()),this,SLOT(slotAccept()));

    connect(m_ui->trend2_bleding,SIGNAL(clicked()),this,SLOT(slotAccept()));


    connect(m_ui->trend2_flow21,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend2_level21,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend2_other21,SIGNAL(clicked()),this,SLOT(slotAccept()));
    connect(m_ui->trend2_term21,SIGNAL(clicked()),this,SLOT(slotAccept()));

}

RHistorySelect::~RHistorySelect()
{
    delete m_ui;
}

void RHistorySelect::slotAccept()
{
    QStringList fl=sender()->objectName().split('_'); // символ 0 є розподілювачам імені таблиці та імені файлу із списком полів
    nameTrend=fl[1];
    // тут треба завантажити дані в структуру 

    QFile f(QString(":/text/%1").arg(nameTrend));
    QString t;
    QStringList sl,sl2;
    QSettings set;

    int nIo = sender()->objectName().mid(5,1).toInt()-1;

    TrendParam->host=set.value("/db/hostname","localhost").toString();
    TrendParam->db=set.value("/db/dbname","test").toString();
    TrendParam->user=set.value("/db/username","scada").toString();
    TrendParam->passwd=set.value("/db/passwd","").toString();
    
    
    if(f.open(QIODevice::ReadOnly))
    {
        int i;
        sl.clear();

        for(i=0;!f.atEnd() && i<8;++i) // обмежети зчитування із файла кінцем файла або не більше як 8 рядків
	{
                t=    QString::fromUtf8(f.readLine()).trimmed();
                //qDebug() << t;
                sl2=t.split('\t');

                TrendParam->fields[i]=t=sl2[0]; // прочитати назву поля
                if(s[nIo]->getTags().contains(t)) // якщо задане поле знайдено
		{
                    sl<< /*s.getText()[t].size() > 0 ? */s[nIo]->getText()[t] /*: t */; // завантажити назву поля, якщо не знайдено - назву тега


                    TrendParam->fScale[i][0]=s[nIo]->scaleZero(t); // спробувати розпізнати тип поля та/чи значення шкали мінімуму
                    TrendParam->fScale[i][1]=s[nIo]->scaleFull(t); // спробувати розпізнати тип поля та/чи значення шкали мінімуму

                     if(s[nIo]->fieldType(t)==1) // якщо дискретний сигнал
                    {
			    // змінити тип поля
                            TrendParam->fields[i]=QString("((%1!=0)*454+%2)").arg(t).arg(i*499);
                            // дискретні шкали
			    TrendParam->fScale[i][0]=0.0-1.1*(double)i;
			    TrendParam->fScale[i][1]=8.8-1.1*(double)i;
			}
		}
		else
                {
                    qDebug() << sl2;
                    if(sl2.size()>3) // якщо дані є  в файлі конфігурації тоді зчитати їх звідти
                    {
                        TrendParam->fScale[i][0]=sl2[1].toDouble(); // прочитати що там є
                        TrendParam->fScale[i][1]=sl2[2].toDouble(); //
                        sl << sl2[3];
                    }
                    else
                        --i;
                } // можливо і поганий варіант яле якщо такого поля не знайдено тоді змінити лічильник циклів
	}

        //qDebug() << "----------";
	TrendParam->numPlot=i; // завантажити кількість графіків
        TrendParam->table=fl[0];
	TrendParam->trend=sender()->objectName(); // Завантажити ім’я тренда
	
	TrendParam->trendHead=qobject_cast<QPushButton*>(sender())->text(); // заголовок тренда - те, що написано на кнопці
	TrendParam->fieldHead = sl;


        //qDebug() << "1 TrendParam->numPlot=" << TrendParam->numPlot;
        //qDebug() << "2 TrendParam->table  =" << TrendParam->table;
        //qDebug() << "3 TrendParam->trend  =" << TrendParam->trend; // Завантажити ім’я тренда
        //for(i=0;i<TrendParam->numPlot;++i)
        //    qDebug() << "4 TrendParam->fields[" << i << "]=" << TrendParam->fields[i];
	
        //for(i=0;i<TrendParam->numPlot;++i)
        //    qDebug() << "5 TrendParam->fScale["<<i<<"] =" << TrendParam->fScale[i][0] << TrendParam->fScale[i][1];
	    
        //qDebug() << "6 TrendParam->trendHead=" << TrendParam->trendHead; // заголовок тренда - те, що написано на кнопці
        //qDebug() << "7 TrendParam->fieldHead =" << TrendParam->fieldHead;

	f.close();
	accept(); // для завершення роботи
    }
    else
	reject(); // якщо не вдалося відкрити відповідний файл


}

void RHistorySelect::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
