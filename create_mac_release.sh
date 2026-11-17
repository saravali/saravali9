#!/bin/bash


# version in configure.ac looks like
version=`grep AC_INIT configure.ac | sed "s/^AC_INIT(\[saravali9\],\[//g; s/\])//g"`

dmgfile="saravali9-${version}.dmg"

appname=Saravali9.app

basedir="saravali9-mac-${version}"
appdir="${basedir}/${appname}"

contentsdir="${appdir}/Contents"
macosdir="${contentsdir}/MacOS"
resourcesdir="${contentsdir}/Resources"

copy_resources()
{
	rname=$1
	# echo "copy resources of type $rname ..."
	if [[ ! -d src/resources/${rname} ]]
	then
		echo "FATAL: directory src/resources/${rname} does not exist"
		exit 1
	fi
	mkdir -p $resourcesdir/$rname
	cp -r src/resources/${rname}/*.json $resourcesdir/${rname}
}

create_dmg()
{
	echo "Creating disk image ${dmgfile}"
	rm -f ${dmgfile}
	hdiutil create -srcfolder ${basedir} ${dmgfile}
	echo "Done"
}

cleanup()
{
	echo "Cleaning directory $basedir"
	rm -fr $basedir
	echo "Done"
}

create_targetdir()
{
	echo "Preparing Mac release in directory $appdir ..."

	mkdir -p ${macosdir}
	mkdir -p ${resourcesdir}

	cp src/gui/saravali9 $macosdir/Saravali9
	cp MacOS/Info.plist $contentsdir
	cp MacOS/PkgInfo $contentsdir
	cp MacOS/readme_macos.txt $basedir

	echo -n "Languages: "
	for l in `cat po/LINGUAS`
	do
		echo -n " $l"
		mkdir -p $resourcesdir/$l
		cp po/$l.gmo $resourcesdir/$l/saravali9.mo
	done
	echo " done."

	echo "Pictures ..."
	mkdir -p $resourcesdir/pic
	cp src/resources/pic/*.jpg $resourcesdir/pic
	cp src/resources/pic/*.png $resourcesdir/pic
	cp -r src/resources/pic/16x16 $resourcesdir/pic
	cp -r src/resources/pic/24x24 $resourcesdir/pic
	cp -r src/resources/pic/32x32 $resourcesdir/pic
	cp src/resources/pic/*.icns $resourcesdir

	mkdir -p $resourcesdir/font
	cp -r src/resources/font/Saravali.ttf $resourcesdir/fonts
	cd $basedir
	ln -fs Saravali9.app/Contents/Resources/font/Saravali.ttf 
	cd -

	# cp COPYING $resourcesdir
	cp src/resources/license.txt $resourcesdir
	cd $basedir
	if test ! -L COPYING
	then
		ln -s ${appname}/Contents/Resources/COPYING
	fi
	cd -

	echo "Locations ..."
	mkdir -p $resourcesdir/atlas
	cp src/resources/atlas/geonames1000.sql $resourcesdir/atlas

	echo "resource files ..."
	# cp -r src/resources/*.json $resourcesdir

	copy_resources "chart"
	copy_resources "dasa"
	copy_resources "print"
	copy_resources "skin"
	copy_resources "yoga"

	if test -L ${appname} 
	then
		rm ${appname}
	fi
	ln -s ${appdir}

	echo "Done"
}

echo "Start $1"
echo "Version is \"$version\" dmg is ${dmgfile}"
echo "Appdir is $appdir"


if [[ $1 == "clean" ]]
then
	cleanup
elif [[ $1 == "dmg" ]]
then
	create_targetdir
	create_dmg
else
	create_targetdir
fi


