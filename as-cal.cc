#include <iostream>
#include <optional>
#include <chrono>
#include <vector>
#include <iomanip>
#include <compare>
#include <boost/date_time.hpp>

namespace greg = boost::gregorian;

#define COLOR_RESET  "\x1b[0m"

struct Task;

template<typename X, typename ...XS>
auto compose(X const& x, XS const& ...xs)
{
	if constexpr (sizeof...(xs) == 0) {
		return x;
	} else {
		return x == 0 ? compose(xs...) : x;
	}
}

struct Task : std::tm
{
	std::string description;

	auto day() const { return tm_mday; }
	auto month() const { return tm_mon + 1; }

	friend auto operator<=>(Task const& lhs, Task const& rhs) -> std::strong_ordering
	{
		return compose(
				lhs.tm_year <=> rhs.tm_year,
				lhs.tm_mon <=> rhs.tm_mon,
				lhs.tm_mday <=> rhs.tm_mday,
				lhs.description <=> rhs.description
		);
	}

	static auto now() -> Task
	{
		auto time = std::time(nullptr);
		auto task = Task{};
		(std::tm&)task = *localtime(&time);
		return task;
	}

	static auto read(std::istream &is = std::cin) -> std::optional<Task>
	{
		using namespace std::chrono_literals;

		// is >> std::get_time(&tm, "%Y.%m.%d");
		std::string line;
		if (!std::getline(is, line))
			return std::nullopt;

		Task task;

		auto time = std::time(nullptr);
		(std::tm&)task = *localtime(&time);
		if (char *after = strptime(line.c_str(), "%d.%m", &task); after) {
			line = line.substr(after - line.c_str() + 1);
		} else {
			perror("as-cal: strptime: ");
			return std::nullopt;
		}

		task.description = line;
		return task;
	}
};

static Task current_task;
static bool interactive = true;

std::ostream& operator<<(std::ostream &os, Task const& task)
{
	os << std::put_time(&task, "%d.%m") << " => " << task.description;
	return os;
}

inline auto dow(auto date)
{
	switch (date.day_of_week()) {
	case 0:  return 6u;
	default: return (date.day_of_week() - 1u) % 7u;
	}
}

inline auto contains(std::vector<Task> const& tasks, auto const& date)
{
	return std::binary_search(std::cbegin(tasks), std::cend(tasks), date,
			[](auto const &a, auto const &b) {
				return a.month() == b.month() ? a.day() < b.day() : a.month() < b.month();
			});
}

unsigned print_calendar(auto year, auto month, std::vector<Task> const& tasks)
{
	char const* days[] = {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su" };
	char const* months[] = {
		"January", "February", "March",
		"April", "May", "June",
		"July", "August", "September",
		"October", "November", "December"
	};

	// auto date = greg::first_day_of_the_week_in_month(greg::Monday, month).get_date(1900 + year);
	year += 1900;
	auto date = greg::date(year, month + 1, 1);

	auto width = 7 * 3;
	auto padding = (width - strlen(months[month]) - 5) / 2;
	std::cout << std::setw(padding) << ' ' << "\x1b[37;1m" << months[month] << ' ' << std::setw(0) << year << '\n';

	std::cout << COLOR_RESET;
	for (auto const& label : days) {
		std::cout << std::setw(3) << std::left << label;
	}
	std::cout << '\n';

	unsigned lines = 2;
	auto day = date;
	auto today = Task::now();
	std::cout << std::setw(dow(day) * 3 + 3) << std::right << "1 ";
	while ((day += greg::date_duration(1)) <= date.end_of_month()) {
		if (day.day() == current_task.day() && day.month() == current_task.month()) {
			std::cout << "\x1b[32;1m";
		}
		else if (contains(tasks, day)) {
			std::cout << "\x1b[36m";
		}

		if (today.day() == day.day() && day.month() == today.month()) {
			std::cout << "\x1b[35;1m";
		}

		std::cout << (day.day() <= 9 ? " " : "") << day.day() << " " COLOR_RESET;
		if (dow(day) >= 6u) {
			lines++;
			std::cout << '\n';
		}
	}
	std::cout << '\n';
	return lines + 1;
}

auto main() -> int
{
	std::vector<Task> tasks;

	for (;;) {
		if (auto maybe_task = Task::read(); maybe_task) {
			tasks.push_back(*maybe_task);
		} else {
			break;
		}
	}

	std::sort(std::begin(tasks), std::end(tasks));

	if (tasks.empty()) {
		return 1;
	}

	auto min = std::lower_bound(std::cbegin(tasks), std::cend(tasks), Task::now());
	if (min == std::cend(tasks))
		return 1;

	tasks.erase(std::cbegin(tasks), min);

	auto min_time = tasks.front();
	auto max_time = tasks.back();

	if (interactive)
		current_task = min_time;

	for (auto year = min_time.tm_year; year <= max_time.tm_year; ++year) {
		for (auto month = min_time.tm_mon; month <= max_time.tm_mon; ++month) {
			auto lines = print_calendar(year, month, tasks);

			auto pmonth = 0, pday = 0;
			for (auto const &task : tasks) {
				if (task.month() - 1 == month && task.tm_year == year) {
					//if (task <=> current_task == 0)
					//	std::cout << "\x1b[7m";
					if (pmonth != task.month() || pday != task.day())
						std::cout << task << "\n" COLOR_RESET;
					else
						std::cout << std::setw(9) << ' ' << task.description << "\n" COLOR_RESET;
					pmonth = task.month(), pday = task.day();
				}
			}

			if (month < max_time.tm_mon || year < max_time.tm_year)
				std::cout << '\n';
		}
	}
}
