set(AWTK_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Path to AWTK root directory")

# 查找awtk库的头文件和库文件
find_library(AWTK_LIBRARY NAMES awtk PATHS ${AWTK_ROOT_DIR}/bin)

file(COPY  ${AWTK_LIBRARY} DESTINATION ${PROJECT_SOURCE_DIR}/bin)

set(AWTK_LIBRARIES ${AWTK_LIBRARY})
set(AWTK_INCLUDE_DIRS ${AWTK_ROOT_DIR}/src ${AWTK_ROOT_DIR}/3rd ${AWTK_ROOT_DIR}/src/ext_widgets ${AWTK_ROOT_DIR}/src)
set(AWTK_DEFINITIONS "-DWITH_FS_RES=1 ")


