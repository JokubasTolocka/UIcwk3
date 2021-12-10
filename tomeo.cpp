//    ______
//   /_  __/___  ____ ___  ___  ____
//    / / / __ \/ __ `__ \/ _ \/ __ \
//   / / / /_/ / / / / / /  __/ /_/ /
//  /_/  \____/_/ /_/ /_/\___/\____/
//              video for sports enthusiasts...
//
//  2811 cw3 : twak 11/11/2021
//

#include <iostream>
#include <fstream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <string>
#include <sstream>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QTextEdit>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QImageReader>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QScrollArea>
#include <QtCore/QDirIterator>
#include <QLineEdit>
#include <QObject>
#include <QLabel>
#include <QSlider>
#include "the_player.h"
#include "the_button.h"
#include "videoslider.h"
#include "playbackspeedselector.h"

using std::cout; using std::cerr;
using std::endl; using std::string;
using std::ifstream; using std::ostringstream;
using std::istringstream;

// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn (std::string loc) {

    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc) );
    QDirIterator it(dir);
    while (it.hasNext()) { // for all files

        QString f = it.next();

        if (f.contains("."))

#if defined(_WIN32)
            if (f.contains(".wmv"))  { // windows
#else
            if (f.contains(".mp4") || f.contains("MOV"))  { // mac/linux
#endif

                QString thumb = f.left( f .length() - 4) +".png";
                if (QFile(thumb).exists()) { // if a png thumbnail exists
                    // Adding this comment to check the Pull Request feature
                    QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                        QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                        out . push_back(TheButtonInfo( url , ico  ) ); // add to the output list
                    }
                    else
                        qDebug() << "warning: skipping video because I couldn't process thumbnail " << thumb << endl;
                }
                else
                    qDebug() << "warning: skipping video because I couldn't find thumbnail " << thumb << endl;
            }
    }

    return out;
}

string readFileIntoString(const string& path) {
    ifstream input_file(path);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << path << "'" << endl;
        exit(EXIT_FAILURE);
    }
    return string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

string getFileName(string filePath, bool withExtension = true, char seperator = '/')
{
    // Get last dot position
    std::size_t dotPos = filePath.rfind('.');
    std::size_t sepPos = filePath.rfind(seperator);
    if(sepPos != std::string::npos)
    {
        return filePath.substr(sepPos + 1, filePath.size() - (withExtension || dotPos != std::string::npos ? 1 : dotPos) );
    }
    return "";
}

std::vector<TheButtonInfo> filterVideos (std::string search, std::vector<TheButtonInfo> videos) {
    std::vector<TheButtonInfo> filteredVideos;

    for (int i = 0; i < videos.size(); i++) {
        string filePath = videos.at(i).url->path().toStdString();
        string videoName = getFileName(filePath);

        if (videoName.find(search) != string::npos) {
            filteredVideos.push_back(videos.at(i));
        }
    }

    return filteredVideos;
}

QString getText(QLineEdit *searchInput) {
    return searchInput->text();
}

int main(int argc, char *argv[]) {

    // let's just check that Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR << endl;

    // create the Qt Application
    QApplication app(argc, argv);

    string filename( std::string(argv[1]) + "//metadata.csv");
    string file_contents;
    file_contents = readFileIntoString(filename);

    string temp = "Video title, short description, tags";
    //convert to Qstring for use in TextEdit
    QString qtemp = QString::fromStdString(file_contents);

    // collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

    if (argc == 2)
        videos = getInfoIn( std::string(argv[1]));

    if (videos.size() == 0) {

        const int result = QMessageBox::question(
                    NULL,
                    QString("Tomeo"),
                    QString("no videos found! download, unzip, and add command line argument to \"quoted\" file location. Download videos from Tom's OneDrive?"),
                    QMessageBox::Yes |
                    QMessageBox::No );

        switch( result )
        {
        case QMessageBox::Yes:
            QDesktopServices::openUrl(QUrl("https://leeds365-my.sharepoint.com/:u:/g/personal/scstke_leeds_ac_uk/EcGntcL-K3JOiaZF4T_uaA4BHn6USbq2E55kF_BTfdpPag?e=n1qfuN"));
            break;
        default:
            break;
        }
        exit(-1);
    }

    //widget for text beneath player
    QTextEdit *description = new QTextEdit;
    description->setText(qtemp);
    description->setReadOnly(true);
    description->setMaximumHeight(70);

    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, QColor(0,128,128,255));
    description->setPalette(pal);

    // the widget that will show the video
    QVideoWidget *videoWidget = new QVideoWidget;

    // the QMediaPlayer which controls the playback
    ThePlayer *player = new ThePlayer;
    player->setVideoOutput(videoWidget);


    //the button bar resting below the video, all widgets within it
    QHBoxLayout *controls = new QHBoxLayout();
    QPushButton *pause = new QPushButton();
    QPushButton *play = new QPushButton();
    QSlider *videoslider = new VideoSlider(Qt::Horizontal);
    QSlider *volumeslider = new QSlider(Qt::Horizontal);
    QPushButton *volumeicon = new QPushButton();
    QPushButton *fullscreen = new QPushButton();


    //adding relevant widgets to control bar, with appropriate settings

    pause->setMaximumWidth(40);
    pause->setIcon(QIcon(":/icons/pause.png"));
    QObject::connect(pause, &QPushButton::released, player, &ThePlayer::pause);
    controls->addWidget(pause);
    play->setMaximumWidth(40);
    play->setIcon(QIcon(":/icons/play.png"));
    QObject::connect(play, &QPushButton::released, player, &ThePlayer::play);
    controls->addWidget(play);

    // add fast-forward button
   QPushButton *fastForward = new QPushButton();
   fastForward->setIcon(QIcon(":/icons/fastforward.png"));
   fastForward->setMaximumWidth(40);
   QObject::connect(fastForward, SIGNAL(released()), player, SLOT(skip5seconds()));
   controls->addWidget(fastForward);

    // found out the issue with below linem, incompatible types and not enough args in signal
    //QObject::connect(player, SIGNAL(durationChanged(qint64)), player, SLOT(setRange(int,int)));
    videoslider->setMinimumWidth(description->width()-280);
//    videoslider->setRange(0, 1000);
//    videoslider->setTickInterval(1);
    QObject::connect(videoslider, SIGNAL(positionChanged(qint64)), player, SLOT(changePosition(qint64)));
    QObject::connect(player, SIGNAL(durationChanged(qint64)), videoslider, SLOT(setMaxRange(qint64)));
    QObject::connect(player, SIGNAL(positionChanged(qint64)), videoslider, SLOT(setPosition(qint64)));

    controls->addWidget(videoslider);
    volumeicon->setMaximumWidth(40);
    volumeicon->setIcon(QIcon(":/icons/volume.png"));
    controls->addWidget(volumeicon);
    volumeslider->setMinimumWidth(160);
    volumeslider->setMaximumWidth(160);
    QObject::connect(volumeslider, SIGNAL(valueChanged(int)), player, SLOT(setVolume(int)));
    controls->addWidget(volumeslider);

    //add fullscreen button
    fullscreen->setMaximumWidth(40);
    fullscreen->setIcon(QIcon(":/icons/fullscreen.png"));
    QObject::connect(fullscreen, SIGNAL(clicked()), player, SLOT(showFullSscreen()));
    controls->addWidget(fullscreen);

    PlaybackSpeedSelector *psSelector = new PlaybackSpeedSelector();
    psSelector->setMaximumWidth(80);
    QObject::connect(psSelector, SIGNAL(speedChanged(qreal)), player, SLOT(setPlaybackRate(qreal)));
    controls->addWidget(psSelector);

    // below is the code for the next and previous buttons, non-functional

    //QPushButton *prevvid = new QPushButton();
    //QPushButton *nextvid = new QPushButton();
    //prevvid->setMaximumWidth(40);
    //QObject::connect(prevvid, &QPushButton::released, player, &ThePlayer::previous);
    //controls->addWidget(prevvid);
    //nextvid->setMaximumWidth(40);
    //QObject::connect(nextvid, &QPushButton::released, player, &ThePlayer::next);
    //controls->addWidget(nextvid);

    //The search on top of displayed videos
    QLineEdit *searchInput = new QLineEdit();
    QPushButton *clearButton = new QPushButton();
    QHBoxLayout *searchBox = new QHBoxLayout();
    searchInput->setPlaceholderText("Search");
    clearButton->setText("Clear");
    // total should be 260, gap is 10
    clearButton->setMaximumWidth(50);
    searchInput->setMaximumWidth(200);

    QString searchValue = "";

    // Once clear button is pressed, the text input is set to be empty
    QObject::connect(clearButton, &QPushButton::clicked, searchInput, &QLineEdit::clear);
    //    QObject::connect(clearButton, &QPushButton::clicked, searchInput, updateSearchValue(searchValue, searchInput));

    // React to changed text. I NEED TO CHANGE searchValue to be searchInput->text() every time a change occurs
    //    QObject::connect(searchInput, &QLineEdit::textChanged, &searchValue, [&searchValue, &searchInput]()->void{searchValue = getText(searchInput);});


    videos = filterVideos(searchValue.toStdString(), videos);

    // Search box is a horizontal layout and it has two widgets: 1) search input 2) clear button
    searchBox->addWidget(searchInput);
    searchBox->addWidget(clearButton);

    // a column of buttons
    QWidget *buttonWidget = new QWidget();
    buttonWidget->setPalette(pal);
    // a list of the buttons
    std::vector<TheButton*> buttons;
    // the buttons are arranged vertically
    QScrollArea *scrollArea= new QScrollArea;
    QVBoxLayout *thumbnailLayout = new QVBoxLayout();
    buttonWidget->setLayout(thumbnailLayout);


    // create the four buttons
    for ( int i = 0; i < videos.size(); i++ ) {
        TheButton *button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL( jumpTo(TheButtonInfo* )), player, SLOT (jumpTo(TheButtonInfo*))); // when clicked, tell the player to play.
        buttons.push_back(button);
        // Get video name and add it above the video -----
        string filePath = videos.at(i).url->path().toStdString();
        string videoName = getFileName(filePath);
        QLabel *videoTitle = new QLabel();
        videoTitle->setText(QString::fromStdString(videoName));
        thumbnailLayout->addWidget(videoTitle);
        // -----------------------------------------------
        button->setStyleSheet("margin-bottom: 20px");
        thumbnailLayout->addWidget(button);
        button->init(&videos.at(i));
    }

    scrollArea->setWidget(buttonWidget);
    scrollArea->setFixedWidth(260);

    QVBoxLayout *videosLayout = new QVBoxLayout();
    videosLayout->addLayout(searchBox);
    videosLayout->addWidget(scrollArea);
    scrollArea->setPalette(pal);

    //right hand side, player and text
    QVBoxLayout *playerLayout = new QVBoxLayout();
    playerLayout->addWidget(videoWidget);
    playerLayout->addLayout(controls);
    playerLayout->addWidget(description);

    // tell the player what buttons and videos are available
    player->setContent(&buttons, & videos);

    // create the main window and layout
    QWidget window;
    QHBoxLayout *view = new QHBoxLayout();
    window.setWindowTitle("tomeo");
    window.setMinimumSize(800, 680);

    // add the video and the buttons to the top level widget
    view->addLayout(videosLayout);
    view->addLayout(playerLayout);

    window.setLayout(view);

    // showtime!
    QPalette pal2 = QPalette();
    pal2.setColor(QPalette::Window, QColor(0,51,51,255));
    window.setPalette(pal2);
    window.show();

    // wait for the app to terminate
    return app.exec();
}
