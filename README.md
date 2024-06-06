# Библиотека для имплементации эвристических алгоритмов на C++

Простой инструмент, позволяющий без лишнего труда настроить и запустить эвристические алгоритмы.

# Алгоритмы

Любой алгоритм в библиотеки наследуется от базового класса:
```C++
template<class InputType, class OutputType>
struct Algorithm
{
    virtual OutputType evolution(InputType&) = 0;
};
```

InputType и OutputType - входные и выходные данные алгоритма соответственно.

Любой алгоритм внутри себя имеет лишь схему работы - иттеративный процесс выполнения его подмодулей в заданом порядке. Подмодули алгоритма называются интерфейсами, они реализуют основные этапы работы любого эвристического алгоритма.

Любой интерфейс наследуется от функтора-обертки:
```C++
template <class ReturnType, class... Args>
struct Wrapper
{
    virtual ReturnType operator()(Args...) = 0;
};
```


## Генетический алгоритм

Основные сущности:

```C++
class GeneType; // Произвольный тип, кодирующий ген в терминах генетического алгоритма
class FitnessType; // Произвольный тип, возвращаемый целевой функцией
```

```C++
template <class GeneType, class FitnessType>
class Chromosome; 
// Класс - хромосома / особь (одно из решений задачи в терминах оптимизации). Обертка над массивом GeneType.

Методы:
(const) std::vector<GeneType>& get(); 
    // Геттер массива генов.
const std::vector<GeneType>& cget() 
    // Константный геттер массива генов.
(const) std::optional<FitnessType>& getFitness()
    // Геттер значения целевой функции особи (если она посчитана).
```

```C++
template <class GeneType, class FitnessType>
class Generation 
// Класс - поколение. Обертка над массивом Chromosome<GeneType, FitnessType>.

Методы:
(const) std::vector<Chromosome<GeneType, FitnessType>>& get(); 
    // Геттер массива хромосом.
```

```C++
template <class GeneType, class FitnessType>
class Population 
// Класс - популяция. Обертка над массивом* Generation<GeneType, FitnessType>.
//* - На самомм деле хранится не весь массив а только его суфикс заданного размера.

Методы:
(const) Suffix<Generation<GeneType, FitnessType>>& get(); 
    // Геттер суфикса массива поколений (итерация в данном типе отсчитывается с конца, другими словами [0] - последний элемент, [1] - предпоследний и т.д.).
```

Сам генетический алгоритм наследуется от Algorithm:
```C++
template<class GeneType, class FitnessType>
struct GeneticAlgorithm: public Algorithm<Population<GeneType, FitnessType>, Population<GeneType, FitnessType>>
```
И имеет следующий список интерфейсов:

```C++
template <class GeneType, class FitnessType>
struct FitnessFunctionWrapper : public Wrapper<FitnessType, const Chromosome<GeneType, FitnessType> &> {};
    // Функтор целевой функции

template <class GeneType, class FitnessType>
struct SelectionWrapper : public Wrapper<void, Generation<GeneType, FitnessType> &> {};
    // Функтор селекции

template <class GeneType, class FitnessType>
struct ConditionsForStoppingWrapper : public Wrapper<bool, const Population<GeneType, FitnessType> &> {};
    // Функтор, отвечающий за остановку алгоритма

template <class GeneType, class FitnessType>
struct MutationWrapper : public Wrapper<void, Generation<GeneType, FitnessType> &> {};
    // Функтор мутации

template <class GeneType, class FitnessType>
struct CrossingoverWrapper : public Wrapper<Generation<GeneType, FitnessType>, const Population<GeneType, FitnessType> &> {};
    // Функтор Кроссинговера

template <class GeneType, class FitnessType>
struct AnyFunctionWrapper : public Wrapper<void, Population<GeneType, FitnessType> &> {};
    // Функтор произвольной функции

template <class GeneType, class FitnessType>
struct StartEvolutionLogWrapper : public Wrapper<void, const Population<GeneType, FitnessType> &> {};
    // Функтор логирования перед началом работы алгоритма

template <class GeneType, class FitnessType>
struct EndEvolutionLogWrapper : public Wrapper<void, const Population<GeneType, FitnessType> &> {};
    // Функтор логировния после конца работы алгоритма

template <class GeneType, class FitnessType>
struct NewGenerationLogWrapper : public Wrapper<void, const Generation<GeneType, FitnessType> &> {};
    // Функтор логирования каждого нового поколения 
```

Чтобы реализовать логику работы нужного интерфейса достаточно унаследоваться от этого интерфейса и перегрузить operator(), где и будет содержаться логира его работы. Чтобы встроить в генетический алгоритм его интерфейсы существет ряд сеттеров:
```C++
void setFitnessFunction(std::shared_ptr<FitnessFunctionWrapper<GeneType, FitnessType>>)
void setSelection(std::shared_ptr<SelectionWrapper<GeneType, FitnessType>>)
void setMutation(std::shared_ptr<MutationWrapper<GeneType, FitnessType>>)
void setCrossingover(std::shared_ptr<CrossingoverWrapper<GeneType, FitnessType>>)
void setConditionsForStopping(std::shared_ptr<ConditionsForStoppingWrapper<GeneType, FitnessType>>)
void setAnyFunction(std::shared_ptr<AnyFunctionWrapper<GeneType, FitnessType>>)

void setStartEvolutionLog(std::shared_ptr<StartEvolutionLogWrapper<GeneType, FitnessType>>)
void setEndEvolutionLog(std::shared_ptr<EndEvolutionLogWrapper<GeneType, FitnessType>>)
void setNewGenerationLog(std::shared_ptr<NewGenerationLogWrapper<GeneType, FitnessType>>)
```

Чтобы запустить генетический алгоритм нужно вызывать метод:
```C++
Population<GeneType, FitnessType> evolution(Population<GeneType, FitnessType>& population) override
```
на вход подается стартовое состояние популяции, а на выходе будет финальное.

Выполнение генетического алгоритма будет выполнятся по следующему правилу:
![Генетический алгоритм](img/evolution.pdf)

Для работы алгоритма необходимы к реализации только FitnessFunctionWrapper и ConditionsForStoppingWrapper, остальные являются опциональными.


## Граф выполнения

Либые эвристические алгоритмы можно произвольным образом комбинировать, получая целые каскады, которые тоже будут эвристическими алгоритмами.

Любой такой каскад - это ориентированный граф без циклов, вершины которого являются эвристическими алгоритмами, а ребра задают порядок выполнения и перенаправляют данные от исходной вершины в конечную.

Вершина:

Так как вершина реализует логику работы любого алгоритма, которые в общем случае работают с различными типами, для работы всего графа необходимо предъявить некоторый тип данных SerializationType, который будет для алгоритма внутри вершины переводится в InputType, а его OutputType будет переводится назад в SerializationType посредством интерфейсов.

```C++
template<class SerializationType>
struct BaseNode
    // Базовый абстрактный класс для любой вершины
{
    BaseNode(std::string id): id_(std::move(id)) {}
    virtual SerializationType evolution(std::vector<SerializationType>&) = 0;
};
```

На вход вершине будет приходить массив SerializationType - выходы предыдущих вершин, которые соедены с этой (нулевой размер будет означать, что это стартовая вершина в графе).
На выходе вершины будет тоже SerializationType;

```C++
template<class SerializationType, class InputType, class OutputType>
struct Node: public BaseNode<SerializationType>;
```

InputType и OutputType соответствуют входному типу и выходному типу алгоритма внутри вершины.
У вершины есть следующий спискок интерфейсов:

```C++
template<class InputType>
struct StartNodeLogWrapper : public Wrapper<void, InputType&, const std::string &> {};
    // Функтор логирования перед началом работы алгоритма

template<class OutputType>
struct EndNodeLogWrapper : public Wrapper<void, OutputType&, const std::string &> {};
    // Функтор логирования после окончания работы алгоритма

template <class InputType>
struct StartNodeFunctionWrapper : public Wrapper<void, InputType&> {};
    // Функтор, который выполняет произвольную логику с входными данными алгоритма

template<class OutputType>
struct EndNodeFunctionWrapper : public Wrapper<void, OutputType&> {};
    // Функтор, который выполняет произвольную логику с выходными данными алгоритма

template<class SerializationType, class InputType>
struct DeserializationWrapper: public Wrapper<InputType, std::vector<SerializationType>&> {};
    // Функтор десериализации входных в вершину данных во входные данные алгоритма

template<class SerializationType, class OutputType>
struct SerializationWrapper: public Wrapper<SerializationType, OutputType&> {};
    // Функтор сериализации выходных данных алгоритма во выходные данные вершины
```

![Схема вершины](img/node.pdf)

Сеттеры:

```C++
void setAlgorithm(std::shared_ptr<Algorithm<InputType, OutputType>>)
void setDeserialization(std::shared_ptr<DeserializationWrapper<SerializationType, InputType>>)
void setStartNodeLog(std::shared_ptr<StartNodeLogWrapper<InputType>>)
void setStartNodeFunction(std::shared_ptr<StartNodeFunctionWrapper<InputType>>)
void setSerialization(std::shared_ptr<SerializationWrapper<SerializationType, OutputType>>)
void setEndNodeLog(std::shared_ptr<EndNodeLogWrapper<OutputType>>)
void setEndNodeFunction(std::shared_ptr<EndNodeFunctionWrapper<OutputType>>)
```

Граф:
```C++
template<class SerializationType>
struct Graph
```

Методы класса:
```C++
void add_node(std::shared_ptr<BaseNode<SerializationType>>)
    // Добавление вершины в граф (с уникальным id)
void add_edge(std::string& from_node, std::string& to_node)
    // Присоеднинение выхода вершины from_node на вход to_node 
void evolution(size_t num_threads = 1)
    // Запуск графа (с заданым количеством потоков)
```

При запуске графа вначале все вершины сортируются топологической сортировкой (если находится цикл, то вызовится исключение). Далее вычисление вершин происходит параллельно с макимальным задданным количеством потоков. 

## Пример

Найдем с помощью генетического алгоритма минимум функции $f(x) = x^4 - 12x^3 + 123x^2 - 1234x + 12345$
В файле [functions](examples/simple/functions.hpp) описана логика всех необходимых интерфейсов.
В файле [main](examples/simple/main.cpp) описано создания генетического алгоритма, вершины графа и сам граф, а таже его вызов.

Пример вывода в консоль:
```txt
----- Start node "node" -----
----- Start evolution -----  
===== cur res: {988121778345.000000;1000.000000} | best res: 988121778345.000000 =====
===== cur res: {351457927.161914;-133.784106} | best res: 351457927.161914 =====
===== cur res: {351457927.161914;-133.784106} | best res: 351457927.161914 =====
===== cur res: {351457927.161914;-133.784106} | best res: 351457927.161914 =====
===== cur res: {152336.842564;22.038321} | best res: 152336.842564 =====
===== cur res: {152336.842564;22.038321} | best res: 152336.842564 =====
===== cur res: {152336.842564;22.038321} | best res: 152336.842564 =====
===== cur res: {152336.842564;22.038321} | best res: 152336.842564 =====
===== cur res: {10740.531863;10.253254} | best res: 10740.531863 =====
===== cur res: {8023.298003;6.338102} | best res: 8023.298003 =====
===== cur res: {8004.854514;6.735018} | best res: 8004.854514 =====
===== cur res: {8004.854514;6.735018} | best res: 8004.854514 =====
===== cur res: {8004.854514;6.735018} | best res: 8004.854514 =====
===== cur res: {8004.854514;6.735018} | best res: 8004.854514 =====
===== cur res: {8004.854514;6.735018} | best res: 8004.854514 =====
===== cur res: {8004.649704;6.709112} | best res: 8004.649704 =====
===== cur res: {8004.625829;6.692205} | best res: 8004.625829 =====
...
===== cur res: {8004.623669;6.695988} | best res: 8004.623669 =====
----- End evolution -----
----- End node "node" -----
```




