// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <QtCore/QModelIndex>
#include <QtQml/QJSValue>
#include <QtCore/QPoint>

class QAbstractItemModel;
class QAction;
class QMenu;
class QWindow;

namespace nx::vms::client::desktop::testkit::utils {

/** Returns true if object properties match with specified properties. */
bool objectMatches(const QObject* object, QJSValue properties);

/** Returns true if text from UI element (with/without shotcuts) matches with specified text. */
bool textMatches(QString itemText, QString text);

/** Returns true if index properties match with specified properties. */
bool indexMatches(QModelIndex index, QJSValue properties);

/**
 * Calls onVisited function for all QModelIndex children of specified parent in the model.
 * If false is returned by onVisited(index) then children of that index are skipped.
 */
void visitModel(
    const QAbstractItemModel* model,
    QModelIndex parent,
    std::function<bool(QModelIndex)> onVisited);

enum VisitOption {
    NoOptions = 0x0,
    OnlyQuickItems = 0x1,
};

using VisitOptions = QFlags<VisitOption>;

/**
 * Calls onVisited function for object itself and all its children.
 * If false is returned by onVisited(object) then children of that object are skipped.
 */
void visitTree(
    QObject* object,
    std::function<bool(QObject*)> onVisited,
    VisitOptions flags = NoOptions);

/**
 * Returns a list of all matching objects.
 */
QVariantList findAllObjects(QJSValue properties);

/**
 * Returns a list of all matching child objects of the container.
 * Adds objects to visited set when they are been checked. Already visited objects are not visited
 * twice.
 */
QVariantList findAllObjectsInContainer(
    QVariant container,
    QJSValue properties,
    QSet<QObject*>& visited);

/**
 * Returns a screen rectangle of QWidget, QQuickItem, QGraphicsObject or QModelIndex bound with
 * visual parent. Returns invalid QRect if the provided object does not have a proper type.
 * Also returns window of the object if it is requested.
 */
QRect globalRect(QVariant object, QWindow** window = nullptr);

/**
 * Returns normalized [0.0 .. 1.0] value of how much area of a sideRect is covered by the orthogonal
 * widgetRect projections. See example below:
 *
 * <pre><code>
 *
 *                              |            |
 *    +- - - - - - - - - - - - -+------------+-
 *    |                         |            |
 *    +---------------+         | widgetRect |
 *    |  intersection |         |            |
 *    +- - - - - - - -+- - - - -+------------+-
 *    |               |         |            |
 *    +---------------+
 *        sideRect
 *
 * </code></pre>
 */
qreal sideIntersect(QRect widgetRect, QRect sideRect, Qt::Alignment align);

enum KeyOption {
    KeyType = 0,
    KeyPress = 1,
    KeyRelease = 2,
};

/**
 * Types specified text into the object.
 * Also supports sequences like `<Enter>`, `<Ctrl+S>` etc.
 */
Qt::KeyboardModifiers sendKeys(
    QJSValue object,
    QString keys,
    KeyOption option = KeyType,
    Qt::KeyboardModifiers modifiers = Qt::NoModifier);

/**
 * Sends mouse event(s) to the specified window.
 */
Qt::MouseButtons sendMouse(
    QWindow* windowHandle,
    QPoint screenPos,
    QString eventType,
    Qt::MouseButton button,
    Qt::KeyboardModifiers modifiers,
    Qt::MouseButtons buttons,
    bool nativeSetPos,
    QPoint pixelDelta = {},
    QPoint angleDelta = {},
    bool inverted = false);

/** Property type or method return type info. */
QVariantMap getMetaInfo(const QMetaObject* meta, QString methodOrProperty);

/** Returns some QModelIndex fields as QVarintMap. */
QVariant dumpQModelIndex(
    const QAbstractItemModel* model,
    QModelIndex parent,
    bool withChildren = false);

/** Returns QObject properties as QVarintMap. */
QVariant dumpQObject(const QObject* object, bool withChildren = false);

/** Returns objectName() for QObject or QML id and source location for QQuickItem. */
std::pair<QString, QString> nameAndBaseUrl(const QObject* object);

/** Returns top level visible QMenu which contains the QAction with valid geometry. */
std::pair<QMenu*, QRect> findActionGeometry(QAction* action);

} // namespace nx::vms::client::desktop::testkit::utils
