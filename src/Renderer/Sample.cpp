#include "Sample.h"
#include "AntiAliasing.h"
namespace rayTracer
{
	std::shared_ptr<SampleAPI> Sample::s_Sampler = SampleAPI::Create(AntialiasingMode::NONE);
}