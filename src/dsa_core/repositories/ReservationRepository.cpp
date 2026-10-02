#include "ReservationRepository.h"

// nhận dữ liệu từ JsonDatabase
    void ReservationRepository:: setdata(const std::vector<reservation>& data){
        reservations=data; //gán reservation=data để chuyển giao dữ liệu từ databaseJson
    }
    //Lấy toàn bộ dữ liệu
    std::vector<reservation>& ReservationRepository:: getall(){
        return reservations;
    }
    // tìm hàng đợi theo mã sách
    reservation* ReservationRepository:: findbookId(std::string bookId){
        for(reservation& res : reservations){
            if (res.bookId==bookId){
                return &res;
            }
        }
        return nullptr;
    }
    void ReservationRepository:: addres(reservation& res){
        reservations.push_back(res);
    }