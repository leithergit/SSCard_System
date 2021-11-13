import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15

import QtQml 2.2

Item {
    id:id_root
    visible: true
    width: 1400
    height: 900
    Rectangle{
        id: id_bk
        color: "white"
        anchors.fill: parent

        Rectangle {
            id:userwindow
            anchors.fill: parent
            //图片地址数组

            property var imagelist:[
                "file:Image/card1.jpg",
                "file:Image/card2.jpg",
                "file:Image/card3.jpg"];
            property var i: 0;

            //图片组件
            Component{
                id:swipeImage;
                Image{
                    asynchronous: true;
                }
            }

            Rectangle{
                id:rect;
                width: parent.width;
                height: parent.height;
                anchors.top: parent.top;
                anchors.topMargin: 0;
                anchors.horizontalCenter: parent.horizontalCenter;
                clip: false;
                SwipeView{
                    id:swipeview;
                    anchors.fill: parent;

                    Timer{//3.5秒后切换图片
                        id:imageSwitch;
                        interval: 3500;
                        repeat: true;
                        onTriggered: {
                            swipeview.currentIndex=(swipeview.currentIndex+1)%userwindow.imagelist.length;
                            indicator.currentIndex=swipeview.currentIndex;
                        }
                    }
                }

                PageIndicator{
                    id:indicator;
                    count:userwindow.imagelist.length
                    interactive: true;
                    anchors.bottom: rect.bottom;
                    anchors.bottomMargin: -6;
                    anchors.horizontalCenter: rect.horizontalCenter;
                    onCurrentIndexChanged: {
                        swipeview.currentIndex=currentIndex;
                    }
                    delegate: Rectangle{
                        width: 10;height: 10
                        radius: 5
                        color: indicator.currentIndex == index ? "green" : "gray"
                    }
                }
            }

            Component.onCompleted: {
                for(i;i<userwindow.imagelist.length;i++)
                {
                    swipeImage.createObject(swipeview,{"source":userwindow.imagelist[i],"x":swipeview.x,"y":swipeview.y,
                                                "width":swipeview.width,"height":swipeview.height});
                }
                swipeview.currentIndex=0;
                imageSwitch.start();
            }
        }
    }
}


