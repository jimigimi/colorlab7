# ColorLab7 (Qt 6, C++)
Задание №7: CMYK ↔ RGB ↔ HSV. Приложение Qt6/Widgets, русский интерфейс.
- Ввод (SpinBox), слайдеры, палитра QColorDialog
- Автопересчёт трёх моделей
- Мягкое предупреждение при ограничении значений
- HEX и предпросмотр

## Сборка (Linux, Arch)
sudo pacman -S qt6-base cmake gcc make
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j
./ColorLab7
