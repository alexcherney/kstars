/*

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef SKYQPAINTER_H
#define SKYQPAINTER_H

#include "skypainter.h"

class DeepSkyObject;


/** @short The QPainter-based painting backend.
    This class implements the SkyPainter interface using a QPainter.
    For documentation, @see SkyPainter. */
class SkyQPainter : public SkyPainter, public QPainter
{
public:
    SkyQPainter(SkyMap *sm);
    virtual ~SkyQPainter();
    virtual void setPen(const QPen& pen);
    virtual void setBrush(const QBrush& brush);
    /** Recalculates the star pixmaps. */
    static void initImages();
    
    // Sky drawing functions
    virtual void drawSkyLine(SkyPoint* a, SkyPoint* b);
    virtual void drawSkyPolyline(LineList* list, SkipList *skipList = 0,
                                 LineListLabel *label = 0);
    virtual void drawSkyPolygon(LineList* list);
    virtual bool drawPointSource(SkyPoint *loc, float mag, char sp = 'A');
    virtual bool drawDeepSkyObject(DeepSkyObject *obj, bool drawImage = false);
    virtual bool drawComet(KSComet *comet);
    virtual bool drawAsteroid(KSAsteroid *ast);
    virtual bool drawPlanet(KSPlanetBase *planet);
    virtual bool drawPlanetMoon(TrailObject *moon);
private:
    ///This function exists so that we can draw other objects (e.g., planets) as point sources.
    virtual void drawPointSource(const QPointF& pos, float size, char sp = 'A');
    virtual void drawDeepSkySymbol(const QPointF& pos, DeepSkyObject* obj,
                                         float positionAngle);
    virtual bool drawDeepSkyImage (const QPointF& pos, DeepSkyObject* obj,
                                         float positionAngle);
};

#endif
