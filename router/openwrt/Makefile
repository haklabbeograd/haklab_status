include $(TOPDIR)/rules.mk

PKG_NAME:=haklab-status
PKG_VERSION_UNSET

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/haklab-status
	SECTION:=utils
	CATEGORY:=misc
	TITLE:=Haklab status
	URL:=https://github.com/haklabbeograd/haklab_status
	DEPENDS:=+libcurl
endef 

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)/usr/bin
	$(MAKE) -C $(PKG_BUILD_DIR) \
	CC=$(TARGET_CC) \
	LIBS=-lcurl \
	CFLAGS="-I. -O2" \
	GIT_VERSION=$$$$(cat $(PKG_BUILD_DIR)/GIT_VERSION)
endef

define Package/haklab-status/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/haklab-status $(1)/usr/bin/
endef

$(eval $(call BuildPackage,haklab-status))
