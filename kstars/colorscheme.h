/***************************************************************************
                          colorscheme.h  -  description
                             -------------------
    begin                : Wed May 8 2002
    copyright            : (C) 2002 by Jason Harris
    email                : kstars@30doradus.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include <tqmap.h>
#include <tqstringlist.h>

class KConfig;

/**
	*@class ColorScheme
	*This class stores all of the adjustable colors in KStars, in 
	*a TQMap object keyed by the names of the colors.  It also stores
	*information on how stars are to be rendered in the map
	*(with realistic colors, or as solid red/whit/black circles).
	*In addition to the brief "Key names" used to index the colors in 
	*the TQMap, each color has a "long name" description that is a bit 
	*more verbose, and suitable for UI display.
	*@author Jason Harris
	*@version 1.0
	*/

class ColorScheme {

	public:

	/**Constructor.  Enter all adjustable colors and their default 
		*values into the TQMap.  Also assign the corresponding long names.
		*/
		ColorScheme();

	/**Copy constructor
		*/
		ColorScheme( const ColorScheme &cs );

	/**Destructor
		*/
		~ColorScheme();

	/**@return true if the Palette tqcontains the given key name
		*/
		bool hasColorNamed( const TQString &name ) const { return ( ! Palette[ name ].isEmpty() ); }
		
	/**
		*@short Retrieve a color by name.  
		*@p name the key name of the color to be retrieved.
		*@return the requested color, or "#FFFFFF" (white) if color name not found. 
		*/
		TQString colorNamed( const TQString &name ) const;
		
	/**@p i the index of the color to retrieve
		*@return a color by its index in the QMap
		*/
		TQString colorAt( int i ) const;
		
	/**@p i the index of the long name to retrieve
		*@return the name of the color at index i
		*/
		TQString nameAt( int i ) const;
		
	/**@p i the index of the key name to retrieve
		*@return the key name of the color at index i
		*/
		TQString keyAt( int i ) const;
		
	/**
		*@return the long name of the color whose key name is given
		*@p key the key name identifying the color.
		*/
		TQString nameFromKey( const TQString &key ) const;
		
	/**Change the color with the given key to the given value
		*@p key the key-name of the color to be changed
		*@p color the new color value
		*/
		void setColor( const TQString &key, const TQString &color );

	/**Load a color scheme from a *.colors file
		*@p filename the filename of the color scheme to be loaded.
		*@return TRUE if the scheme was successfully loaded
		*/
		bool load( const TQString &filename );
		
	/**Save the current color scheme to a *.colors file.
		*@p name the filename to create
		*@return TRUE if the color scheme is successfully writeen to a file
		*/
		bool save( const TQString &name );
		
	/**@return the Filename associated with the color scheme.
		*/
		TQString fileName() const { return FileName; }
		
	/**Copy a color scheme
		*@p cs the color scheme to be copied into this object
		*/
		void copy( const ColorScheme &cs );

	/**Read color-scheme data from the Config object.
		*/
		void loadFromConfig( KConfig* );
		
	/**Save color-scheme data to the Config object.
		*/
		void saveToConfig( KConfig* );

	/**@return the number of colors in the color scheme.*/
		unsigned int numberOfColors() const { return (int)Palette.size(); }

	/**@return the star color mode used by the color scheme*/
		int starColorMode() const { return StarColorMode; }
		
	/**@return the star color intensity value used by the color scheme*/
		int starColorIntensity() const { return StarColorIntensity; }
		
	/**Set the star color mode used by the color scheme*/
		void setStarColorMode( int mode ) { StarColorMode = mode; }
		
	/**Set the star color intensity value used by the color scheme*/
		void setStarColorIntensity( int intens) { StarColorIntensity = intens; }

	private:
		int StarColorMode, StarColorIntensity;
		TQString FileName;
		TQStringList KeyName, Name, Default;
		TQMap<TQString,TQString> Palette;

};

#endif
