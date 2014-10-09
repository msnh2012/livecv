/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/
#include "QStateContainer.hpp"
#include <QQuickView>
#include <QQuickItem>

QStateContainerManager* QStateContainerManager::m_instance = 0;

/**
 * @brief QStateContainerManager singleton instance
 * @param item
 * @param parent
 * @return
 */
QStateContainerManager &QStateContainerManager::instance(QQuickItem *item, QObject* parent){
    if ( m_instance == 0 )
        m_instance = new QStateContainerManager(item, parent);
    return *m_instance;
}

/**
 * @brief QStateContainerManager constructor
 * @param item
 * @param parent
 */
QStateContainerManager::QStateContainerManager(QQuickItem *item, QObject *parent)
    : QObject(parent){

    atexit(&cleanStateManager);

    connect(item, SIGNAL(windowChanged(QQuickWindow*)), SLOT(attachWindow(QQuickWindow*)));
}

/**
 * @brief Calls beforeCompile() on all of its children
 */
void QStateContainerManager::beforeCompile(){
    for ( QLinkedList<QStateContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->beforeCompile();
}

/**
 * @brief Calls afterCompile() on all of its children
 */
void QStateContainerManager::afterCompile(){
    for ( QLinkedList<QStateContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->afterCompile();
}

/**
 * @brief Called when the gui initializes a window
 * @param window
 */
void QStateContainerManager::attachWindow(QQuickWindow *window){
    if (window != 0){
        QQuickView* view = qobject_cast<QQuickView*>(window);
        if ( view == 0 )
            qDebug() << "Cannot connect to window compilation signal. State objects will not get destroyed, "
                     << "which will cause the application to slow down over time.";
        else {
            connect(view->rootObject(), SIGNAL(beforeCompile()), SLOT(beforeCompile()));
            connect(view->rootObject(), SIGNAL(afterCompile()),  SLOT(afterCompile()));
        }
    }
}

/**
 * @brief QStateContainerManager destructor
 */
QStateContainerManager::~QStateContainerManager(){
    for ( QLinkedList<QStateContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        delete *it;
    m_stateContainerList.clear();
}

/**
 * @brief Register a state container to the manager.
 * @param container
 */
void QStateContainerManager::registerStateContainer(QStateContainerBase *container){
    m_stateContainerList.append(container);
}

/**
 * @brief Cleanup function.
 */
void QStateContainerManager::cleanStateManager() {
    delete m_instance;
    m_instance = 0;
}

/**
 * @brief QStateContainerBase destructor
 */
QStateContainerBase::QStateContainerBase(){
}
