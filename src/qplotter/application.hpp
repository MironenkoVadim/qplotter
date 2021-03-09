#ifndef APPLICATION_HPP_
#define APPLICATION_HPP_

#include <QApplication>
#include <QDateTime>
#include <QTimer>


class Application : public QApplication
{
	Q_OBJECT


public:
	// argc обязательно передавать по ссылке
	Application( int& argc, char** argv );
	~Application() override;
	void handlePosixSignal( int signal );

	Q_SLOT void start();

private:
	QDateTime m_startTime;

	void readSettings();
}; // class Application

#endif /* APPLICATION_HPP_ */
