wget http://deb.debian.org/debian/pool/main/b/budgie-desktop/budgie-desktop_10.5.orig.tar.xz
wget http://deb.debian.org/debian/pool/main/b/budgie-desktop/budgie-desktop_10.5-1.debian.tar.xz
xzcat budgie-desktop_10.5.orig.tar.xz | tar xvf -
cd budgie-desktop-10.5/
xzcat ../budgie-desktop_10.5-1.debian.tar.xz | tar xvf -
sudo apt-get install build-essential devscripts lintian
sudo apt install gir1.2-gee-0.8 gnome-common gobject-introspection gtk-doc-tools libgee-0.8-dev libgnome-bluetooth-dev libgnome-desktop-3-dev libgnome-menu-3-dev libgtk-3-dev libmutter-3-dev  libpeas-dev libpolkit-agent-1-dev libpolkit-gobject-1-dev libupower-glib-dev libwnck-3-dev libgdk-pixbuf2.0-dev libaccountsservice-dev valac gnome-settings-daemon-dev sassc libnotify-dev
debuild 
