/*  FITS Label
    Copyright (C) 2003-2017 Jasem Mutlaq <mutlaqja@ikarustech.com>
    Copyright (C) 2016-2017 Robert Lancaster <rlancaste@gmail.com>

    This application is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
*/

#pragma once

#include "fitscommon.h"

#include <config-kstars.h>

#ifdef HAVE_DATAVISUALIZATION
#include "starprofileviewer.h"
#endif

#include <QFutureWatcher>
#include <QPixmap>
#include <QScrollArea>
#include <QStack>
#include <QPointer>

#ifdef WIN32
// avoid compiler warning when windows.h is included after fitsio.h
#include <windows.h>
#endif

#include <fitsio.h>

#include <memory>

#define MINIMUM_PIXEL_RANGE 5
#define MINIMUM_STDVAR      5

class QAction;
class QEvent;
class QGestureEvent;
class QImage;
class QLabel;
class QPinchGesture;
class QResizeEvent;
class QToolBar;

class FITSData;
class FITSLabel;

class FITSView : public QScrollArea
{
        Q_OBJECT
    public:
        explicit FITSView(QWidget *parent = nullptr, FITSMode fitsMode = FITS_NORMAL, FITSScale filterType = FITS_NONE);
        virtual ~FITSView() override;

        typedef enum {dragCursor, selectCursor, scopeCursor, crosshairCursor } CursorMode;

        /**
         * @brief loadFITS Loads FITS data and display it in FITSView frame
         * @param inFilename FITS File name
         * @param silent if set, error popups are suppressed.
         * @note If image is successfully, loaded() signal is emitted, otherwise failed() signal is emitted.
         * Obtain error by calling lastError()
         */
        void loadFITS(const QString &inFilename, bool silent = true);
        // Save FITS
        int saveFITS(const QString &newFilename);
        // Rescale image lineary from image_buffer, fit to window if desired
        bool rescale(FITSZoom type);

        // Access functions
        FITSData *getImageData() const
        {
            return imageData;
        }
        double getCurrentZoom() const
        {
            return currentZoom;
        }
        QImage getDisplayImage() const
        {
            return rawImage;
        }
        const QPixmap &getDisplayPixmap() const
        {
            return displayPixmap;
        }

        // Tracking square
        void setTrackingBoxEnabled(bool enable);
        bool isTrackingBoxEnabled() const
        {
            return trackingBoxEnabled;
        }
        QPixmap &getTrackingBoxPixmap(uint8_t margin = 0);
        void setTrackingBox(const QRect &rect);
        const QRect &getTrackingBox() const
        {
            return trackingBox;
        }

        // last error
        const QString &lastError() const
        {
            return m_LastError;
        }

        // Overlay
        virtual void drawOverlay(QPainter *);

        // Overlay objects
        void drawStarCentroid(QPainter *);
        void drawTrackingBox(QPainter *);
        void drawMarker(QPainter *);
        void drawCrosshair(QPainter *);
        void drawEQGrid(QPainter *);
        void drawObjectNames(QPainter *painter);
        void drawPixelGrid(QPainter *painter);

        bool isCrosshairShown();
        bool areObjectsShown();
        bool isEQGridShown();
        bool isPixelGridShown();
        bool imageHasWCS();

        void updateFrame();

        bool isTelescopeActive();

        void enterEvent(QEvent *event) override;
        void leaveEvent(QEvent *event) override;
        CursorMode getCursorMode();
        void setCursorMode(CursorMode mode);
        void updateMouseCursor();

        void updateScopeButton();
        void setScopeButton(QAction *action)
        {
            centerTelescopeAction = action;
        }

        // Zoom related
        void cleanUpZoom(QPoint viewCenter);
        QPoint getImagePoint(QPoint viewPortPoint);
        uint16_t zoomedWidth()
        {
            return currentWidth;
        }
        uint16_t zoomedHeight()
        {
            return currentHeight;
        }

        // Star Detection
        int findStars(StarAlgorithm algorithm = ALGORITHM_CENTROID, const QRect &searchBox = QRect());
        void toggleStars(bool enable);
        void setStarsEnabled(bool enable);
        void setStarFilterRange(float const innerRadius, float const outerRadius);
        int filterStars();

        // FITS Mode
        void updateMode(FITSMode fmode);
        FITSMode getMode()
        {
            return mode;
        }

        void setFilter(FITSScale newFilter)
        {
            filter = newFilter;
        }

        void setFirstLoad(bool value);

        void pushFilter(FITSScale value)
        {
            filterStack.push(value);
        }
        FITSScale popFilter()
        {
            return filterStack.pop();
        }

        CursorMode lastMouseMode { selectCursor };
        bool isStarProfileShown()
        {
            return showStarProfile;
        }
        // Floating toolbar
        void createFloatingToolBar();

        //void setLoadWCSEnabled(bool value);

    public slots:
        void wheelEvent(QWheelEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;
        void ZoomIn();
        void ZoomOut();
        void ZoomDefault();
        void ZoomToFit();

        // Grids
        void toggleEQGrid();
        void toggleObjects();
        void togglePixelGrid();
        void toggleCrosshair();

        // Stars
        void toggleStars();
        void toggleStarProfile();
        void viewStarProfile();

        void centerTelescope();

        virtual void processPointSelection(int x, int y);
        virtual void processMarkerSelection(int x, int y);
        void move3DTrackingBox(int x, int y);
        void resizeTrackingBox(int newSize);

    protected slots:
        /**
             * @brief syncWCSState Update toolbar and actions depending on whether WCS is available or not
             */
        void syncWCSState();

        bool event(QEvent *event) override;
        bool gestureEvent(QGestureEvent *event);
        void pinchTriggered(QPinchGesture *gesture);

    protected:
        template <typename T>
        bool rescale(FITSZoom type);

        double average();
        double stddev();
        void calculateMaxPixel(double min, double max);
        void initDisplayImage();

        QPointF getPointForGridLabel();
        bool pointIsInImage(QPointF pt, bool scaled);

        void loadInFrame();

        /// WCS Future Watcher
        QFutureWatcher<bool> wcsWatcher;
        /// FITS Future Watcher
        QFutureWatcher<bool> fitsWatcher;
        /// Cross hair
        QPointF markerCrosshair;
        /// Pointer to FITSData object
        FITSData *imageData { nullptr };
        /// Current zoom level
        double currentZoom { 0 };

    private:
        QLabel *noImageLabel { nullptr };
        QPixmap noImage;

        QVector<QPointF> eqGridPoints;

        std::unique_ptr<FITSLabel> image_frame;

        uint32_t image_width { 0 };
        uint32_t image_height { 0 };

        /// Current width due to zoom
        uint16_t currentWidth { 0 };
        uint16_t lastWidth { 0 };
        /// Current height due to zoom
        uint16_t currentHeight { 0 };
        uint16_t lastHeight { 0 };
        /// Image zoom factor
        const double zoomFactor;

        // Original full-size image
        QImage rawImage;
        // Scaled images
        QImage scaledImage;
        // Actual pixmap after all the overlays
        QPixmap displayPixmap;

        bool firstLoad { true };
        bool markStars { false };
        bool showStarProfile { false };
        bool showCrosshair { false };
        bool showObjects { false };
        bool showEQGrid { false };
        bool showPixelGrid { false };

        struct
        {
            bool used() const
            {
                return innerRadius != 0.0f || outerRadius != 1.0f;
            }
            float innerRadius { 0.0f };
            float outerRadius { 1.0f };
        } starFilter;

        CursorMode cursorMode { selectCursor };
        bool zooming { false };
        int zoomTime { 0 };
        QPoint zoomLocation;

        QString filename;
        FITSMode mode;
        FITSScale filter;
        QString m_LastError;

        QStack<FITSScale> filterStack;

        // Tracking box
        bool trackingBoxEnabled { false };
        QRect trackingBox;
        QPixmap trackingBoxPixmap;

        // Scope pixmap
        QPixmap redScopePixmap;
        // Magenta Scope Pixmap
        QPixmap magentaScopePixmap;

        // Floating toolbar
        QToolBar *floatingToolBar { nullptr };
        QAction *centerTelescopeAction { nullptr };
        QAction *toggleEQGridAction { nullptr };
        QAction *toggleObjectsAction { nullptr };
        QAction *toggleStarsAction { nullptr };
        QAction *toggleProfileAction { nullptr };

        //Star Profile Viewer
#ifdef HAVE_DATAVISUALIZATION
        QPointer<StarProfileViewer> starProfileWidget;
#endif

    signals:
        void newStatus(const QString &msg, FITSBar id);
        void debayerToggled(bool);
        void wcsToggled(bool);
        void actionUpdated(const QString &name, bool enable);
        void trackingStarSelected(int x, int y);
        void loaded();
        void failed();
        void starProfileWindowClosed();

        friend class FITSLabel;
};
