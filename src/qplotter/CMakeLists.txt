# Название основной цели в текущем каталоге
set(TRGT qplotter)

# Список файлов исходных текстов
set(TRGT_cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/application.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/main_window.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/marks_plot.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt_plot_extcurve.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt_plot_picker_ext.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt_point_ext.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt_series_data_ext.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/surok_marks_plotter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/surok_preprocessing.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/surok_exchange.cpp
  )

set(TRGT_moc_hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/application.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/main_window.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/marks_plot.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt_plot_picker_ext.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/surok_marks_plotter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/surok_preprocessing.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/surok_exchange.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/check_list.hpp
  )

set(TRGT_hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt_point_ext.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt_plot_extcurve.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qwt_series_data_ext.hpp
  )

set(TRGT_ui
  ${CMAKE_CURRENT_SOURCE_DIR}/main_window.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/surok_marks_plotter.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/surok_preprocessing.ui
  )

set(TRGT_headers ${TRGT_hpp} ${TRGT_moc_hpp})
###
# Конец списков файлов
###
# cmake-format: on

# Правило для автоматической генерации препроцессором moc
qt5_wrap_cpp(TRGT_moc_cpp ${TRGT_moc_hpp})

#
qt5_wrap_ui(TRGT_uih ${TRGT_ui})

# Правила для создания файла ресурсов с вложенными файлами переводов
qt5_translation(
  TRGT_qrc_cpp
  OUTPUT_DIR ${CMAKE_SOURCE_DIR}/l10n
  BASE_NAME ${TRGT}
  SOURCES ${TRGT_cpp} ${TRGT_ui} ${TRGT_headers}
  LANGUAGES ru_RU)

# Правило для автоматической генерации препроцессором qrc
# (обязательно после вызова функции qt5_translation, если она есть,
# так как она добавляет свои файлы к списку ресурсов)
qt5_add_resources(TRTG_qrc_cpp ${TRGT_qrc})

# Путь поиска библиотек внутри проекта
link_directories(${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

# Цель для создания исполняемого файла
add_executable(${TRGT} ${TRGT_moc_cpp} ${TRGT_uih} ${TRGT_qrc_cpp} ${TRGT_cpp} ${TRGT_headers})
common_target_properties(${TRGT})

# Создание цели format-sources для автоматического формирования кода
add_format_sources(${TRGT} ${TRGT_cpp} ${TRGT_headers})

# Создание цели clazy-check для проверки утилитой clazy
add_clazy_check(${TRGT} ${TRGT_cpp} ${TRGT_headers})

# Создание цели clang-tidy-check для проверки утилитой clang-tidy
add_clang_tidy_check(${TRGT} ${TRGT_cpp} ${TRGT_headers})

# Создание цели clang-analyze-check для проверки утилитой clang-analyze
add_clang_analyze_check(${TRGT} ${TRGT_cpp} ${TRGT_headers})

# Создание цели pvs-check для проверки утилитой pvs-studio-analyzer
add_pvs_check(${TRGT})

add_dependencies(${TRGT} veer-protocols myxlib)

# Qt5 подключение заголовочных файлов
target_include_directories(${TRGT} SYSTEM PUBLIC ${Qt5Core_INCLUDE_DIRS})
target_include_directories(${TRGT} SYSTEM PUBLIC ${Qt5Gui_INCLUDE_DIRS})
target_include_directories(${TRGT} SYSTEM PUBLIC ${Qt5Widgets_INCLUDE_DIRS})
target_include_directories(${TRGT} SYSTEM PUBLIC ${Qt5OpenGL_INCLUDE_DIRS})
target_include_directories(${TRGT} SYSTEM PUBLIC ${Qt5Network_INCLUDE_DIRS})

target_include_directories(${TRGT} SYSTEM PUBLIC ${QWT_INCLUDE_DIR})
target_include_directories(${TRGT} SYSTEM PUBLIC ${QWTPLOT3D_INCLUDE_DIR})
target_include_directories(${TRGT} SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(${TRGT} ${QWTPLOT3D_LIBRARY} ${QWT_LIBRARY})

target_link_libraries(${TRGT} myx-filesystem myx-qt)
target_link_libraries(${TRGT} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::Network)
target_link_libraries(${TRGT} Threads::Threads)

# Имя целевого каталога и выходного файла для цели
set_target_properties(${TRGT} PROPERTIES OUTPUT_NAME ${TRGT})

# Подключение настроек для динамического анализа программы
add_sanitizers(${TRGT})

# Подключение возможности использования утилиты Gcov
# для исследования покрытия кода
add_code_coverage(${TRGT})

# Подключение возможности включения пакетного режима обработки
# исходных файлов в генераторах для ускорения сборки
cotire(${TRGT})

# Правила для установки
install(TARGETS ${TRGT} COMPONENT MAIN RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
