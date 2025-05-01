#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <algorithm>

struct CrosswordWord {
    std::string word;
    std::string definition;
    int startX, startY;
    bool isHorizontal;
};

class Crossword {
private:
    std::vector<std::vector<char>> grid;
    std::vector<CrosswordWord> words;
    std::unordered_map<int, CrosswordWord*> numberedWords;
    std::string name;
    
public:
    Crossword(int size, const std::string& name) : grid(size, std::vector<char>(size, ' ')), name(name) {}
    
    void addWord(const std::string& word, const std::string& definition, 
                 int x, int y, bool horizontal, int number) {
        CrosswordWord cw{word, definition, x, y, horizontal};
        words.push_back(cw);
        numberedWords[number] = &words.back();
        
        if (horizontal) {
            for (int i = 0; i < word.size(); ++i) {
                grid[y][x + i] = toupper(word[i]);
            }
        } else {
            for (int i = 0; i < word.size(); ++i) {
                grid[y + i][x] = toupper(word[i]);
            }
        }
    }
    
    const std::vector<std::vector<char>>& getGrid() const { return grid; }
    const std::vector<CrosswordWord>& getWords() const { return words; }
    CrosswordWord* getWordByNumber(int number) {
        if (numberedWords.count(number)) return numberedWords[number];
        return nullptr;
    }
    int size() const { return grid.size(); }
    std::string getName() const { return name; }
    
    bool checkSolution(const std::vector<std::vector<char>>& userAnswers) const {
        for (int y = 0; y < grid.size(); ++y) {
            for (int x = 0; x < grid.size(); ++x) {
                if (grid[y][x] != ' ' && toupper(userAnswers[y][x]) != grid[y][x]) {
                    return false;
                }
            }
        }
        return true;
    }
};

class CrosswordApp {
private:
    sf::RenderWindow window;
    sf::Font font;
    
    enum class Screen { MAIN_MENU, CROSSWORD, WIN_SCREEN };
    Screen currentScreen = Screen::MAIN_MENU;
    
    std::vector<Crossword> crosswords;
    int currentLevel = 0;
    std::vector<std::vector<char>> userAnswers;
    
    sf::Vector2i selectedCell = {-1, -1};
    bool horizontalSelected = true;
    bool showError = false;
    bool isCompleted = false;
    
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
    
    // ... (остальные методы остаются такими же, как в предыдущем коде)

    void handleCrosswordEvents(sf::Event& event) {
        if (event.type == sf::Event::KeyPressed) {
            if (selectedCell.x != -1 && selectedCell.y != -1) {
                if (event.key.code >= sf::Keyboard::A && event.key.code <= sf::Keyboard::Z) {
                    char c = 'A' + (event.key.code - sf::Keyboard::A);
                    userAnswers[selectedCell.y][selectedCell.x] = c;
                    
                    if (horizontalSelected) {
                        selectedCell.x = (selectedCell.x + 1) % crosswords[currentLevel].size();
                    } else {
                        selectedCell.y = (selectedCell.y + 1) % crosswords[currentLevel].size();
                    }
                    
                    // Проверка решения после каждого ввода
                    if (crosswords[currentLevel].checkSolution(userAnswers)) {
                        isCompleted = true;
                        currentScreen = Screen::WIN_SCREEN;
                    }
                } 
                else if (event.key.code == sf::Keyboard::BackSpace) {
                    userAnswers[selectedCell.y][selectedCell.x] = ' ';
                }
                else if (event.key.code == sf::Keyboard::Space) {
                    horizontalSelected = !horizontalSelected;
                }
                else if (event.key.code == sf::Keyboard::Enter) {
                    if (crosswords[currentLevel].checkSolution(userAnswers)) {
                        isCompleted = true;
                        currentScreen = Screen::WIN_SCREEN;
                    } else {
                        showError = true;
                    }
                }
            }
        }
        // ... (остальная обработка событий)
    }

    void renderWinScreen() {
        sf::RectangleShape background(sf::Vector2f(600, 400));
        background.setPosition(100, 100);
        background.setFillColor(sf::Color(240, 240, 240));
        background.setOutlineThickness(2);
        background.setOutlineColor(sf::Color::Black);
        window.draw(background);

        sf::Text congratsText("Поздравляем!", font, 40);
        congratsText.setFillColor(sf::Color::Green);
        congratsText.setPosition(250, 150);
        window.draw(congratsText);

        sf::Text levelText("Вы успешно завершили:\n" + crosswords[currentLevel].getName(), font, 30);
        levelText.setFillColor(sf::Color::Black);
        levelText.setPosition(200, 220);
        window.draw(levelText);

        sf::RectangleShape nextButton(sf::Vector2f(200, 50));
        nextButton.setPosition(300, 350);
        nextButton.setFillColor(sf::Color(180, 230, 180));
        nextButton.setOutlineThickness(2);
        nextButton.setOutlineColor(sf::Color::Black);
        window.draw(nextButton);

        sf::Text nextText("Следующий уровень", font, 20);
        nextText.setFillColor(sf::Color::Black);
        nextText.setPosition(320, 365);
        window.draw(nextText);
    }

    // ... (другие методы)

    void render() {
        window.clear(sf::Color::White);
        
        switch (currentScreen) {
            case Screen::MAIN_MENU: renderMainMenu(); break;
            case Screen::CROSSWORD: renderCrossword(); break;
            case Screen::WIN_SCREEN: renderWinScreen(); break;
        }
        
        window.display();
    }
};

int main() {
    CrosswordApp app;
    app.run();
    return 0;
}
