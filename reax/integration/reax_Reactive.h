#pragma once

// If you get a compiler error here, it means that you are trying to use an unsupported type T.
template<typename T, typename Enable = void>
class Reactive;
