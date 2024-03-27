#include <string>




namespace tcp_handler{

    class Handler {
    public:
        Handler(const Handler&) = delete;
        Handler& operator=(const Handler&) = delete;

        Handler() {}

        template <typename Send>
        void operator()(std::string client_data, Send&& sender) {
            std::cout << client_data;
            sender("Message has been delivered\n");
        }

    private:


    };


}
