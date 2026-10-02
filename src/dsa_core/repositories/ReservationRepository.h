#pragma once
#include <vector>
#include <string>
#include "../models/Reservation.h"
class ReservationRepository
{ 
private:
    std::vector<reservation> reservations;
public:
    // nhận dữ liệu từ JsonDatabase
    void setdata(const std::vector<reservation>& data);
    //Lấy toàn bộ dữ liệu
    std::vector<reservation>& getall();
    // tìm hàng đợi theo mã sách
    reservation* findbookId(std::string bookId);
    // thêm reservation mới
    void addres(reservation& res);
    
};