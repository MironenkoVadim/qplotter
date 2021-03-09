#ifndef MAIN_WINDOW_HPP_
#define MAIN_WINDOW_HPP_

#include "marks_plot.hpp"
#include "qwt_plot_extcurve.hpp"
#include "surok_exchange.hpp"
#include "surok_marks_plotter.hpp"
#include "surok_preprocessing.hpp"
#include "ui_main_window.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT

public:
	explicit MainWindow( QMainWindow* parent = nullptr );
	~MainWindow();

private:
	QTimer enabledTimer;

	SurokExchange      m_surokExchange;
	SurokMarksPlotter  m_surokMarkPlotter;
	SurokPreprocessing m_surokPreprocessing;

	void defaultSettings();
	Q_SLOT void graphDisplay();
	Q_SLOT void enabledTab();
	Q_SLOT void connection();
}; // class MainWindow

#endif // ifndef MAIN_WINDOW_HPP_
