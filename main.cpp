#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

// Структура для хранения информации о слове в кроссворде
struct CrosswordWord {
    std::string word;       // Само слово
    std::string definition; // Определение (подсказка)
    int startX, startY;     // Стартовая позиция
    bool isHorizontal;      // Горизонтальное или вертикальное
};

// Класс кроссворда
class Crossword {
private:
    std::vector<std::vector<char>> grid; // Сетка кроссворда
    std::vector<CrosswordWord> words;    // Слова кроссворда
    std::unordered_map<int, CrosswordWord*> numberedWords; // Слова с номерами
    
public:
    Crossword(int size) : grid(size, std::vector<char>(size, ' ')) {}
    
    // Добавление слова в кроссворд
    void addWord(const std::string& word, const std::string& definition, 
                 int x, int y, bool horizontal, int number) {
        CrosswordWord cw{word, definition, x, y, horizontal};
        words.push_back(cw);
        numberedWords[number] = &words.back();
        
        if (horizontal) {
            for (int i = 0; i < word.size(); ++i) {
                grid[y][x + i] = word[i];
            }
        } else {
            for (int i = 0; i < word.size(); ++i) {
                grid[y + i][x] = word[i];
            }
        }
    }
    
    // Получение сетки
    const std::vector<std::vector<char>>& getGrid() const {
        return grid;
    }
    
    // Получение слов
    const std::vector<CrosswordWord>& getWords() const {
        return words;
    }
    
    // Получение слова по номеру
    CrosswordWord* getWordByNumber(int number) {
        if (numberedWords.count(number)) {
            return numberedWords[number];
        }
        return nullptr;
    }
    
    // Размер кроссворда
    int size() const {
        return grid.size();
    }
};

// Класс приложения
class CrosswordApp {
private:
    sf::RenderWindow window;
    sf::Font font;
    
    enum class Screen { MAIN_MENU, CROSSWORD };
    Screen currentScreen = Screen::MAIN_MENU;
    
    Crossword crossword;
    int currentLevel = 1;
    
    // Пользовательские ответы
    std::vector<std::vector<char>> userAnswers;
    
    // Выбранная ячейка
    sf::Vector2i selectedCell = {-1, -1};
    bool horizontalSelected = true;
    
public:
    CrosswordApp() : window(sf::VideoMode(800, 600), "Кроссворд"), crossword(15) {
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Ошибка загрузки шрифта\n";
        }
        
        // Инициализация кроссвордов (уровней)
        initCrosswords();
        userAnswers.resize(crossword.size(), std::vector<char>(crossword.size(), ' '));
    }
    
    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }
    
private:
    void initCrosswords() {
        // Уровень 1 - Основы программирования
        crosswords.emplace_back(15, "Основы программирования");
        auto& cw1 = crosswords.back();
        cw1.addWord("ПРОГРАММИРОВАНИЕ", "Процесс создания компьютерных программ", 0, 0, true, 1);
        cw1.addWord("АЛГОРИТМ", "Последовательность действий для решения задачи", 0, 2, false, 2);
        cw1.addWord("СИНТАКСИС", "Правила написания кода", 3, 4, true, 3);
        cw1.addWord("КОМПИЛЯТОР", "Программа, переводящая код в машинный язык", 5, 0, false, 4);
        cw1.addWord("ПЕРЕМЕННАЯ", "Именованная область памяти", 7, 6, true, 5);
        cw1.addWord("ФУНКЦИЯ", "Подпрограмма, выполняющая конкретную задачу", 9, 0, false, 6);

        // Уровень 2 - Структуры данных
        crosswords.emplace_back(15, "Структуры данных");
        auto& cw2 = crosswords.back();
        cw2.addWord("МАССИВ", "Набор элементов одного типа", 2, 1, true, 1);
        cw2.addWord("СПИСОК", "Последовательность связанных элементов", 4, 0, false, 2);
        cw2.addWord("ОЧЕРЕДЬ", "Структура FIFO (первый пришел - первый вышел)", 0, 3, true, 3);
        cw2.addWord("СТЕК", "Структура LIFO (последний пришел - первый вышел)", 6, 0, false, 4);
        cw2.addWord("ДЕРЕВО", "Иерархическая структура данных", 1, 5, true, 5);
        cw2.addWord("ГРАФ", "Совокупность вершин и ребер", 8, 2, false, 6);

        // Уровень 3 - Базы данных
        crosswords.emplace_back(15, "Базы данных");
        auto& cw3 = crosswords.back();
        cw3.addWord("СУБД", "Система управления базами данных", 0, 0, true, 1);
        cw3.addWord("ЗАПРОС", "Команда для получения данных", 0, 2, false, 2);
        cw3.addWord("ТРАНЗАКЦИЯ", "Логическая единица работы с БД", 0, 4, true, 3);
        cw3.addWord("ИНДЕКС", "Средство ускорения поиска", 5, 1, false, 4);
        cw3.addWord("НОРМАЛИЗАЦИЯ", "Процесс оптимизации структуры БД", 2, 6, true, 5);
        cw3.addWord("РЕЛЯЦИОННАЯ", "Модель данных на основе таблиц", 3, 0, false, 6);
    }
    
    }
    
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if (currentScreen == Screen::MAIN_MENU) {
                handleMainMenuEvents(event);
            } else if (currentScreen == Screen::CROSSWORD) {
                handleCrosswordEvents(event);
            }
        }
    }
    
    void handleMainMenuEvents(sf::Event& event) {
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(
                    event.mouseButton.x, event.mouseButton.y));
                
                // Проверка нажатия на кнопки уровней
                for (int i = 1; i <= 3; ++i) {
                    sf::FloatRect levelRect(300, 150 + i * 80, 200, 50);
                    if (levelRect.contains(mousePos)) {
                        currentLevel = i;
                        currentScreen = Screen::CROSSWORD;
                        resetCrossword();
                        initCrosswords();
                    }
                }
            }
        }
    }
    
    void handleCrosswordEvents(sf::Event& event) {
        if (event.type == sf::Event::KeyPressed) {
            if (selectedCell.x != -1 && selectedCell.y != -1) {
                if (event.key.code >= sf::Keyboard::A && event.key.code <= sf::Keyboard::Z) {
                    char c = 'a' + (event.key.code - sf::Keyboard::A);
                    userAnswers[selectedCell.y][selectedCell.x] = c;
                    
                    // Перемещаемся к следующей ячейке
                    if (horizontalSelected) {
                        selectedCell.x = (selectedCell.x + 1) % crossword.size();
                    } else {
                        selectedCell.y = (selectedCell.y + 1) % crossword.size();
                    }
                } else if (event.key.code == sf::Keyboard::BackSpace) {
                    userAnswers[selectedCell.y][selectedCell.x] = ' ';
                    
                    // Перемещаемся к предыдущей ячейке
                    if (horizontalSelected) {
                        selectedCell.x = (selectedCell.x - 1 + crossword.size()) % crossword.size();
                    } else {
                        selectedCell.y = (selectedCell.y - 1 + crossword.size()) % crossword.size();
                    }
                } else if (event.key.code == sf::Keyboard::Space) {
                    // Переключаем направление
                    horizontalSelected = !horizontalSelected;
                }
            }
        } else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(
                    event.mouseButton.x, event.mouseButton.y));
                
                // Проверка нажатия на кнопку "Назад"
                sf::FloatRect backButton(20, 20, 100, 40);
                if (backButton.contains(mousePos)) {
                    currentScreen = Screen::MAIN_MENU;
                    return;
                }
                
                // Проверка нажатия на ячейку кроссворда
                int cellSize = 30;
                int startX = (window.getSize().x - crossword.size() * cellSize) / 2;
                int startY = 100;
                
                int gridX = (mousePos.x - startX) / cellSize;
                int gridY = (mousePos.y - startY) / cellSize;
                
                if (gridX >= 0 && gridX < crossword.size() && 
                    gridY >= 0 && gridY < crossword.size()) {
                    selectedCell = sf::Vector2i(gridX, gridY);
                }
            }
        }
    }
    
    void update() {
        // Обновление логики приложения
    }
    
    void render() {
        window.clear(sf::Color::White);
        
        if (currentScreen == Screen::MAIN_MENU) {
            renderMainMenu();
        } else if (currentScreen == Screen::CROSSWORD) {
            renderCrossword();
        }
        
        window.display();
    }
    
    void renderMainMenu() {
        sf::Text title("Кроссворд", font, 50);
        title.setFillColor(sf::Color::Black);
        title.setPosition(300, 50);
        window.draw(title);
        
        sf::Text subtitle("Выберите уровень", font, 30);
        subtitle.setFillColor(sf::Color::Black);
        subtitle.setPosition(300, 100);
        window.draw(subtitle);
        
        // Кнопки уровней
        for (int i = 1; i <= 3; ++i) {
            sf::RectangleShape levelButton(sf::Vector2f(200, 50));
            levelButton.setPosition(300, 150 + i * 80);
            levelButton.setFillColor(sf::Color(200, 200, 200));
            levelButton.setOutlineThickness(2);
            levelButton.setOutlineColor(sf::Color::Black);
            window.draw(levelButton);
            
            sf::Text levelText("Уровень " + std::to_string(i), font, 24);
            levelText.setFillColor(sf::Color::Black);
            levelText.setPosition(350, 160 + i * 80);
            window.draw(levelText);
        }
    }
    
    void renderCrossword() {
        // Кнопка "Назад"
        sf::RectangleShape backButton(sf::Vector2f(100, 40));
        backButton.setPosition(20, 20);
        backButton.setFillColor(sf::Color(200, 200, 200));
        backButton.setOutlineThickness(2);
        backButton.setOutlineColor(sf::Color::Black);
        window.draw(backButton);
        
        sf::Text backText("Назад", font, 20);
        backText.setFillColor(sf::Color::Black);
        backText.setPosition(45, 25);
        window.draw(backText);
        
        // Заголовок уровня
        sf::Text levelTitle("Уровень " + std::to_string(currentLevel), font, 30);
        levelTitle.setFillColor(sf::Color::Black);
        levelTitle.setPosition(350, 50);
        window.draw(levelTitle);
        
        // Отрисовка кроссворда
        int cellSize = 30;
        int startX = (window.getSize().x - crossword.size() * cellSize) / 2;
        int startY = 100;
        
        // Номера слов
        std::unordered_map<std::string, int> wordNumbers;
        int currentNumber = 1;
        for (const auto& word : crossword.getWords()) {
            std::string pos = std::to_string(word.startX) + "," + std::to_string(word.startY);
            if (!wordNumbers.count(pos)) {
                wordNumbers[pos] = currentNumber++;
            }
        }
        
        // Ячейки кроссворда
        for (int y = 0; y < crossword.size(); ++y) {
            for (int x = 0; x < crossword.size(); ++x) {
                sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
                cell.setPosition(startX + x * cellSize, startY + y * cellSize);
                cell.setOutlineThickness(1);
                cell.setOutlineColor(sf::Color::Black);
                
                // Проверка, является ли ячейка частью слова
                bool isWordCell = false;
                for (const auto& word : crossword.getWords()) {
                    if (word.isHorizontal) {
                        if (y == word.startY && x >= word.startX && x < word.startX + word.word.size()) {
                            isWordCell = true;
                            break;
                        }
                    } else {
                        if (x == word.startX && y >= word.startY && y < word.startY + word.word.size()) {
                            isWordCell = true;
                            break;
                        }
                    }
                }
                
                if (isWordCell) {
                    cell.setFillColor(sf::Color::White);
                } else {
                    cell.setFillColor(sf::Color::Black);
                }
                
                window.draw(cell);
                
                // Номера слов
                std::string pos = std::to_string(x) + "," + std::to_string(y);
                if (wordNumbers.count(pos)) {
                    sf::Text numberText(std::to_string(wordNumbers[pos]), font, 10);
                    numberText.setFillColor(sf::Color::Black);
                    numberText.setPosition(startX + x * cellSize + 2, startY + y * cellSize + 2);
                    window.draw(numberText);
                }
                
                // Буквы (пользовательский ввод)
                if (userAnswers[y][x] != ' ') {
                    sf::Text letterText(std::string(1, userAnswers[y][x]), font, 20);
                    letterText.setFillColor(sf::Color::Blue);
                    letterText.setPosition(
                        startX + x * cellSize + cellSize/2 - letterText.getLocalBounds().width/2,
                        startY + y * cellSize + cellSize/2 - letterText.getLocalBounds().height/2 - 5);
                    window.draw(letterText);
                }
            }
        }
        
        // Выделение текущей ячейки
        if (selectedCell.x != -1 && selectedCell.y != -1) {
            sf::RectangleShape highlight(sf::Vector2f(cellSize, cellSize));
            highlight.setPosition(
                startX + selectedCell.x * cellSize, 
                startY + selectedCell.y * cellSize);
            highlight.setFillColor(sf::Color::Transparent);
            highlight.setOutlineThickness(2);
            highlight.setOutlineColor(sf::Color::Red);
            window.draw(highlight);
            
            // Подсказка
            for (const auto& word : crossword.getWords()) {
                if ((word.isHorizontal && selectedCell.y == word.startY && 
                     selectedCell.x >= word.startX && selectedCell.x < word.startX + word.word.size()) ||
                    (!word.isHorizontal && selectedCell.x == word.startX && 
                     selectedCell.y >= word.startY && selectedCell.y < word.startY + word.word.size())) {
                    
                    sf::Text hintText("Подсказка: " + word.definition, font, 16);
                    hintText.setFillColor(sf::Color::Black);
                    hintText.setPosition(50, window.getSize().y - 50);
                    window.draw(hintText);
                    break;
                }
            }
        }
    }
    
    void resetCrossword() {
        crossword = Crossword(15);
        userAnswers.clear();
        userAnswers.resize(crossword.size(), std::vector<char>(crossword.size(), ' '));
        selectedCell = {-1, -1};
    }
};

int main() {
    CrosswordApp app;
    app.run();
    return 0;
}
