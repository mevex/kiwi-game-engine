#include "core/logger.h"

int main(void)
{
	LogFatal("This is a test message: %.2f", 3.14f);
	LogError("This is a test message: %.2f", 3.14f);
	LogWarning("This is a test message: %.2f", 3.14f);
	LogInfo("This is a test message: %.2f", 3.14f);
	LogDebug("This is a test message: %.2f", 3.14f);
	LogTrace("This is a test message: %.2f", 3.14f);

	return 0;
}