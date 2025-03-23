#ifndef ISERVICE_H
#define ISERVICE_H

namespace ctask::service
{
    /**
     * @interface IService
     * @brief Abstract service interface.
     *
     * Defines a common contract for all services with lifecycle management.
     * Any service should be able to start and stop.
     *
     * Why?
     * - Keeps services independent of specific implementations.
     * - Makes it easier to manage multiple services in a uniform way.
     * - Enables mocking in tests for clean dependency injection.
     */
    class IService
    {
    public:
        IService() = default;
        IService(const IService&) = delete;
        IService(IService&&) = delete;
        IService& operator=(const IService&) = delete;
        IService& operator=(IService&&) = delete;

        virtual ~IService() = default;

        /**
         * @brief Starts the service.
         *
         * If the service is already running, this should be a no-op or throw.
         */
        virtual void start() = 0;

        /**
         * @brief Stops the service.
         *
         * Should properly stop service.
         */
        virtual void stop() = 0;
    };
}

#endif //ISERVICE_H
