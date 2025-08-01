cmake_minimum_required(VERSION 3.16)

project(RadikPass VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql UiTools LinguisTools Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql UiTools LinguistTools Gui)

set(TS_FILES
  resources/translations/uk.ts
  resources/translations/ge.ts
  resources/translations/en.ts
)

set(PROJECT_SOURCES
  Main.cpp
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt6_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
    qt_add_executable(RadikPass
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${QM_FILES}
        EntryAdder.h EntryAdder.cpp EntryAdder.ui
        DbManager.h DbManager.cpp
        TableAdder.h TableAdder.cpp TableAdder.ui
        DbOpener.h DbOpener.cpp DbOpener.ui
        DbCreator.h DbCreator.cpp DbCreator.ui
        EntryEditor.h EntryEditor.cpp EntryEditor.ui
        CellDelegate.h
        ColumnsConfigurator.h ColumnsConfigurator.cpp ColumnsConfigurator.ui


        TableEditor.h TableEditor.cpp TableEditor.ui

        resources.qrc
        IconLoader.h
        PasswordGenerator.h PasswordGenerator.cpp PasswordGenerator.ui

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET RadikPass APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(RadikPass SHARED
            ${PROJECT_SOURCES}
            ${QM_FILES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(RadikPass
            ${PROJECT_SOURCES}
        )
    endif()
endif()


target_link_libraries(RadikPass PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::UiTools
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::GuiPrivate
)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.RadikPass)
endif()
set_target_properties(RadikPass PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

set(OPENSSL_ROOT_DIR "C:/Qt/Tools/OpenSSLv3/Win_x64")
set(OPENSSL_LIBRARIES "${OPENSSL_ROOT_DIR}/lib")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")

find_package(OpenSSL)
if(OPENSSL_FOUND)
    target_link_libraries(RadikPass PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    message(STATUS "found openssl: ${OPENSSL_VERSION}")
else()
    message(WARNING "Openssl is not found!")
endif()

if(MINGW)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc")
endif()

include(GNUInstallDirs)
install(TARGETS RadikPass
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RadikPass)
endif()
