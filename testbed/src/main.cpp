#include "core/logger.h"
#include "platform/platform.h"

int main(void)
{
	LogFatal("This is a test message: %.2f", 3.14f);
	LogError("This is a test message: %.2f", 3.14f);
	LogWarning("This is a test message: %.2f", 3.14f);
	LogInfo("This is a test message: %.2f", 3.14f);
	LogDebug("This is a test message: %.2f", 3.14f);
	LogTrace("This is a test message: %.2f", 3.14f);

	Platform::State State = {};
	if (Platform::Startup(&State, "Kiwi Testbed", 200, 200, 1280, 720))
	{
		b8 Running = true;
		while (Running)
		{
			Platform::ProcessMessageQueue(&State);
			Platform::SleepMS(16);
		}
	}
	Platform::Terminate(&State);

	return 0;
}