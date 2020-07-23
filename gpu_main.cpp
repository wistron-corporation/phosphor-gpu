#include <sdbusplus/bus.hpp>
#include <sdbusplus/sdbus.hpp>
#include <sdbusplus/server/manager.hpp>
#include <string.h>
#include "gpu_manager.hpp"
#include <set>
#include <fstream>
#include <phosphor-logging/log.hpp>
#include <phosphor-logging/elog-errors.hpp>

using namespace phosphor::logging;

int main(void)
{

    sdbusplus::bus::bus bus = sdbusplus::bus::new_default();

    sd_event* event = nullptr;
    auto eventDeleter = [](sd_event* e) { e = sd_event_unref(e); };
    using SdEvent = std::unique_ptr<sd_event, decltype(eventDeleter)>;
    // acquire a reference to the default event loop
    sd_event_default(&event);
    SdEvent sdEvent{event, eventDeleter};
    event = nullptr;
    // attach bus to this event loop
    bus.attach_event(sdEvent.get(), SD_EVENT_PRIORITY_NORMAL);

    sdbusplus::server::manager::manager objManager(bus, GPU_OBJ_PATH_ROOT);

    phosphor::gpu::Gpu objMgr(bus, GPU_OBJ_PATH_ROOT);

    sdbusplus::server::manager::manager service_objManager(bus, GPU_OBJ_PATH_SERVICE_ROOT);

    phosphor::gpu::Gpu service_objMgr(bus, GPU_OBJ_PATH_SERVICE_ROOT);

    bus.request_name(GPU_REQUEST_NAME);

    objMgr.run();

    // Start event loop for all sd-bus events

    sd_event_loop(bus.get_event());

    bus.detach_event();

    return 0;
}
