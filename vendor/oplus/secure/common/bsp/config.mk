##################################################################################
## OPLUS_FEATURE_SECURITY_COMMON
## Copyright (C), 2008-2018, OPLUS Mobile Comm Corp., Ltd
##
## Description:
##      fingerprints_hal config
##
## Version: 2.0
## Date created: 2017/11/02
## TAG: BSP.Fingerprint.Basic
##
## --------------------------- Revision History: --------------------------------
##  <author>      <data>            <desc>
##  Ziqing.Guo    2017/11/16        Add alipay service
##  Bin.Li        2018/03/12        add for devicebinding of trustonic tee in MTK6771
##  Hongdao.yu    2018/04/27        remake config to different repos
##  Long.Liu      2018/12/12        add libTBaseProvisioningAT for VTS test fail
##  Long.Liu      2018/12/24        add for migrate fingerprint and face template
##  oujinrong     2019/07/03        remove product macro
##################################################################################

CUSTOM_IMAGE_MODULES += init.oplus.seccommon.rc

CUSTOM_IMAGE_MODULES += librpmbengclient
CUSTOM_IMAGE_MODULES += rpmbengclient_test
# CUSTOM_IMAGE_MODULES += TBaseDeviceBinding
# CUSTOM_IMAGE_MODULES += libTBaseProvisioning
# CUSTOM_IMAGE_MODULES += libTBaseProvisioningAT
