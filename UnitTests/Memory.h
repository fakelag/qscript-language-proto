#include <string>
#ifdef __linux__
#include <sys/sysinfo.h>
#endif

#ifdef __APPLE__
#include <mach/task.h>
#include <mach/mach_init.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <Psapi.h>
#else
#include <sys/resource.h>
#endif

namespace Memory
{
	std::string FormatBytes( size_t bytes )
	{
		char szBuffer[ 64 ];

		if ( bytes >= 1e9 )
			snprintf( szBuffer, sizeof( szBuffer ), "%.2f Gb", ( double ) bytes / 1e9f );
		else if ( bytes >= 1e6 )
			snprintf( szBuffer, sizeof( szBuffer ), "%.2f Mb", ( double ) bytes / 1e6f );
		else if ( bytes >= 1e3 )
			snprintf( szBuffer, sizeof( szBuffer ), "%.2f Kb", ( double ) bytes / 1e3f );
		else
			snprintf( szBuffer, sizeof( szBuffer ), "%lu bytes", bytes );

		return std::string( szBuffer );
	}
	size_t GetVirtualMemoryUsage ()
	{
#if defined(__linux__)
		size_t size = 0;
		FILE *file = fopen("/proc/self/statm", "r");
		if (file)
		{
			unsigned long vm = 0;
			fscanf (file, "%ul", &vm);
			fclose (file);
			size = (size_t)vm * getpagesize();
		}

		return size;
#elif defined(__APPLE__)
		task_basic_info t_info;
		mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
		task_info( current_task(), TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count );
		return t_info.resident_size;
#elif defined(_WIN32)
		PROCESS_MEMORY_COUNTERS counters;

		if ( GetProcessMemoryInfo ( GetCurrentProcess(), &counters, sizeof(counters) ) )
			return counters.PagefileUsage;

		return 0;

#else
		throw "Unknown platform";
#endif
	}
}
