#include "webview.h"

#include <QDebug>
#include <QTimer>
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <QProgressBar>

#include <QStyle>

ProgressBar::ProgressBar(QWidget *parent)
    : QFrame(parent),
      m_progress(0)
{
    setAutoFillBackground(true);

    setBackgroundRole(QPalette::Window);

    setFrameStyle(QFrame::StyledPanel | QFrame::Plain);

    QWidget *header = new QWidget(this);
    m_label = new QLabel(tr("Loading..."), header);
    m_stop = new QPushButton(header);
    m_stop->setFlat(true);
    m_stop->setFixedSize(QSize(22,22));
    m_stop->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    connect(m_stop, SIGNAL(pressed()), this, SLOT(hide()));

    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setMargin(0);
    headerLayout->addWidget(m_label);
    headerLayout->addWidget(m_stop);
    header->setLayout(headerLayout);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(header);
    layout->addWidget(m_progressBar);
    setLayout(layout);
}

ProgressBar::~ProgressBar()
{
}

QSize ProgressBar::sizeHint() const
{
    QSize size(300, QFrame::sizeHint().height());
    return size;
}

void ProgressBar::startLoad()
{
    m_progressBar->setValue(m_progress);
    show();
}

void ProgressBar::changeLoad(int change)
{
    m_progress = change;
    m_progressBar->setValue(change);
}

void ProgressBar::endLoad()
{
    QTimer::singleShot(1000, this, SLOT(hide()));
    m_progress = 0;
}


WebView::WebView(QWidget *parent)
    : QWebView(parent)
{
    m_progressBar = new ProgressBar(this);
    m_progressBar->setGeometry(20, 20, m_progressBar->sizeHint().width(),
                                       m_progressBar->sizeHint().height());
}

WebView::~WebView()
{
}

void WebView::moveEvent(QMoveEvent *event)
{
    updateProgressGeometry();
    QWebView::moveEvent(event);
}

void WebView::resizeEvent(QResizeEvent *event)
{
    updateProgressGeometry();
    QWebView::resizeEvent(event);
}

void WebView::updateProgressGeometry()
{
    m_progressBar->setGeometry((width() - m_progressBar->width()) / 2 + pos().x(),
                               height() - int(m_progressBar->height() * 2.5) + pos().y(),
                               m_progressBar->width(),
                               m_progressBar->height());
}
