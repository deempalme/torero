#ifndef TORERO_TERMINAL_PRINTER_H
#define TORERO_TERMINAL_PRINTER_H

#include "torero/definition/types.h"

#include <boost/regex.hpp>
#include <string>
#include <stdarg.h>

namespace torero {
  namespace terminal {
    class Printer
    {
    public:
      Printer();

      /*
     * ### Sets quick ANSI parameters
     *
     * For more information look at this
     * [link](https://www.wikiwand.com/en/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters)
     *
     * @parameters
     * {const int} parameter = ANSI parameter's number
     *
     */
      static void add_style(const int parameter = torero::terminal::color::Style::Reset);

      static void attention(const std::string &text);
      static void attention(const char *text);
      static void attention(const bool boolean);
      static void attention(const char character);
      static void attention(const unsigned char character);
      static void attention(const signed char character);
      static void attention(const int integer_number);
      static void attention(const unsigned int unsigend_integer_number);
      static void attention(const short int short_integer_number);
      static void attention(const unsigned short int unsigned_short_integer_number);
      static void attention(const long int long_integer_number);
      static void attention(const unsigned long int unsigned_long_integer_number);
      static void attention(const float float_number);
      static void attention(const double double_number);
      static void attention(const long double long_double_number);
      /*
     * ### Cleans the terminal/console
     *
     * Cleans all characters in the terminal/console
     *
     * @parameters
     * {const bool} force = if `true` deletes also all lines saved in the scrollback history
     *
     */
      static void clean(const bool force = false);
      /*
     * ### Colorize the text or background using 8-bit colors
     *
     * For more information about the 8-bit color codes look at this
     * [link](https://www.wikiwand.com/en/ANSI_escape_code#8-bit)
     *
     * @parameters
     * {const int} color = ANSI 8-bit color's code
     * {const int} usage = Foreground or Background
     *
     */
      static void colorize(const int color,
                           const int usage = torero::terminal::color::Style::Foreground);
      /*
     * ### Colorize the text or background using 24-bit colors
     *
     * For more information about the 24-bit color codes look at this
     * [link](https://www.wikiwand.com/en/ANSI_escape_code#24-bit)
     *
     * @parameters
     * {const int} color = ANSI 24-bit color's code
     * {const int} usage = Foreground or Background
     *
     */
      static void colorize(const int red, const int green, const int blue,
                           const int usage = torero::terminal::color::Style::Foreground);

      static void error(const std::string &text);
      static void error(const char *text);
      static void error(const bool boolean);
      static void error(const char character);
      static void error(const unsigned char character);
      static void error(const signed char character);
      static void error(const int integer_number);
      static void error(const unsigned int unsigend_integer_number);
      static void error(const short int short_integer_number);
      static void error(const unsigned short int unsigned_short_integer_number);
      static void error(const long int long_integer_number);
      static void error(const unsigned long int unsigned_long_integer_number);
      static void error(const float float_number);
      static void error(const double double_number);
      static void error(const long double long_double_number);
      /*
     * ### Displaying messages in Terminal/Console with std::printf style format
     *
     * This function displays messages into the Ubuntu Terminal or Window's console. They
     * are colored depending on its type:
     *   - Error: Red
     *   - Warning: yellow
     *   - Attention: Blue
     *   - Normal: green
     *
     * It works similar to `std::printf`; `const char *format` should contain how to interpret
     * the data, look at [https://en.cppreference.com/w/cpp/io/c/fprintf]\
     * (https://en.cppreference.com/w/cpp/io/c/fprintf) to see all possible formats.
     *
     * @parameters
     * {const char*} format = Pointer to a null-terminated multibyte string specifying how
     *                        to interpret the data.
     * {const int} message_type = Type of message to display.
     * {...} = variable argument list (multiple variables, multiple types)
     *
     */
      static void formatted(const char *format, const torero::Message::MessageType type, ...)
      __attribute__((format (printf, 1, 3)));
      /*
     * ### Displaying messages in Terminal/Console
     *
     * This function displays messages into the Ubuntu Terminal or Window's console. They
     * are colored depending on its type:
     *   - Error: Red
     *   - Warning: yellow
     *   - Attention: Blue
     *   - Normal: green
     *
     * @parameters
     * {const std::string&} text = Text to display in Terminal/Console.
     * {const int} message_type = Type of message to display.
     *
     */
      static void message(const std::string &text,
                          const torero::Message::MessageType type = torero::Message::Error);
      static void message(const char *text,
                          const torero::Message::MessageType type = torero::Message::Error);
      static void message(const bool boolean,
                          const torero::Message::MessageType type = torero::Message::Error);
      static void message(const char character,
                          const torero::Message::MessageType type = torero::Message::Error);
      static void message(const unsigned char character,
                          const torero::Message::MessageType type = torero::Message::Error);
      static void message(const signed char character,
                          const torero::Message::MessageType type = torero::Message::Error);
      static void message(const int integer_number,
                          const torero::Message::MessageType type = torero::Message::Error);
      static void message(const unsigned int unsigend_integer_number,
                          const torero::Message::MessageType type = torero::Message::Error);
      static void message(const short int short_integer_number,
                          const torero::Message::MessageType type = torero::Message::Error);
      static void message(const unsigned short int unsigned_short_integer_number,
                          const torero::Message::MessageType type = torero::Message::Error);
      static void message(const long int long_integer_number,
                          const torero::Message::MessageType type = torero::Message::Error);
      static void message(const unsigned long int unsigned_long_integer_number,
                          const torero::Message::MessageType type = torero::Message::Error);
      static void message(const float float_number,
                          const torero::Message::MessageType type = torero::Message::Error);
      static void message(const double double_number,
                          const torero::Message::MessageType type = torero::Message::Error);
      static void message(const long double long_double_number,
                          const torero::Message::MessageType type = torero::Message::Error);

      static void normal(const std::string &text);
      static void normal(const char *text);
      static void normal(const bool boolean);
      static void normal(const char character);
      static void normal(const unsigned char character);
      static void normal(const signed char character);
      static void normal(const int integer_number);
      static void normal(const unsigned int unsigend_integer_number);
      static void normal(const short int short_integer_number);
      static void normal(const unsigned short int unsigned_short_integer_number);
      static void normal(const long int long_integer_number);
      static void normal(const unsigned long int unsigned_long_integer_number);
      static void normal(const float float_number);
      static void normal(const double double_number);
      static void normal(const long double long_double_number);
      /*
     * ### Resets the text format in Terminal/Console
     *
     * Deletes all previous text formats and sets all defaults
     *
     */
      static void reset();

      static void warning(const std::string &text);
      static void warning(const char *text);
      static void warning(const bool boolean);
      static void warning(const char character);
      static void warning(const unsigned char character);
      static void warning(const signed char character);
      static void warning(const int integer_number);
      static void warning(const unsigned int unsigend_integer_number);
      static void warning(const short int short_integer_number);
      static void warning(const unsigned short int unsigned_short_integer_number);
      static void warning(const long int long_integer_number);
      static void warning(const unsigned long int unsigned_long_integer_number);
      static void warning(const float float_number);
      static void warning(const double double_number);
      static void warning(const long double long_double_number);

    private:
      static void add_format(const int parameter, const std::string &settings = "");
      static void typify(const torero::Message::MessageType type);
      static std::string current_format_;
    };
  }
}
//#define formated(format, type, ...) _formated(format"\n", type, ##__VA_ARGS__)

#endif // TORERO_TERMINAL_PRINTER_H
