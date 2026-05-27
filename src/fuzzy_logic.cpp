#include "fuzzy_logic.h"

// Hàm tính toán thời gian khóa dựa trên thuật toán TRUE FUZZY LOGIC (Mô hình mờ Sugeno)
unsigned long calculateFuzzyTimeout(int wrongs, unsigned long timeDiff) {
    
    // ==========================================
    // BƯỚC 1: MỜ HÓA (FUZZIFICATION)
    // ==========================================
    
    // 1.1 Mờ hóa biến Tốc độ thao tác (timeDiff - khoảng cách giữa 2 lần sai tính bằng mili-giây)
    // Định nghĩa tập mờ: NHANH (dưới 6s) và CHẬM (trên 2s)
    float mu_Nhanh = 0.0;
    float mu_Cham = 0.0;

    if (timeDiff <= 6000) {
        mu_Nhanh = 1.0;
        mu_Cham = 0.0;
    } 
    else if (timeDiff > 6000 && timeDiff < 10000) {
        // Áp dụng hàm thuộc tính hình thang/tam giác để tính độ khớp mờ
        mu_Nhanh = (10000.0 - timeDiff) / (10000.0 - 6000.0);
        mu_Cham = (timeDiff - 6000.0) / (10000.0 - 6000.0);
    } 
    else {
        mu_Nhanh = 0.0;
        mu_Cham = 1.0;
    }

    // 1.2 Mờ hóa biến Số lần sai (wrongs)
    // Định nghĩa tập mờ đơn điểm (Singletons): ÍT (1 lần), VỪA (2 lần), NHIỀU (>= 3 lần)
    float mu_It = (wrongs == 1) ? 1.0 : 0.0;
    float mu_Vua = (wrongs == 2) ? 1.0 : 0.0;
    float mu_Nhieu = (wrongs >= 3) ? 1.0 : 0.0;

    // ==========================================
    // BƯỚC 2: ĐỊNH NGHĨA GIÁ TRỊ NGÕ RA (SINGLETONS)
    // ==========================================
    unsigned long z_Ngan = 2500;   // Tiêu chuẩn 2.5 giây cho loa nói xong
    unsigned long z_Vua = 15000;   // Phạt vừa 15 giây
    unsigned long z_Dai = 60000;   // Phong tỏa toàn diện 60 giây (1 phút)

    // ==========================================
    // BƯỚC 3: ĐÁNH GIÁ LUẬT MỜ (FUZZY INFERENCE RULES)
    // ==========================================
    // Sử dụng toán tử MIN cho phép GIAO (AND)
    
    // Luật 1: NẾU Sai Ít THÌ Phạt Ngắn
    float w1 = mu_It;
    
    // Luật 2: NẾU Sai Vừa VÀ Thao tác Nhanh THÌ Phạt Vừa
    float w2 = min(mu_Vua, mu_Nhanh);
    
    // Luật 3: NẾU Sai Vừa VÀ Thao tác Chậm THÌ Phạt Ngắn
    float w3 = min(mu_Vua, mu_Cham);
    
    // Luật 4: NẾU Sai Nhiều VÀ Thao tác Nhanh THÌ Phạt Dài (Kẻ trộm mò mã)
    float w4 = min(mu_Nhieu, mu_Nhanh);
    
    // Luật 5: NẾU Sai Nhiều VÀ Thao tác Chậm THÌ Phạt Vừa (Người nhà luống cuống)
    float w5 = min(mu_Nhieu, mu_Cham);

    // ==========================================
    // BƯỚC 4: GIẢI MỜ (DEFUZZIFICATION)
    // ==========================================
    // Áp dụng công thức Trung bình trọng số: Tử số / Mẫu số
    float tu_so = (w1 * z_Ngan) + (w2 * z_Vua) + (w3 * z_Ngan) + (w4 * z_Dai) + (w5 * z_Vua);
    float mau_so = w1 + w2 + w3 + w4 + w5;

    unsigned long penaltyTime = z_Ngan; // Giá trị phòng hờ mặc định
    if (mau_so > 0) {
        penaltyTime = (unsigned long)(tu_so / mau_so);
    }

    // In thông số mờ ra Serial Monitor để Tân đưa dữ liệu chạy thực tế vào báo cáo đồ án
    Serial.printf("\n--- [TOÁN TỬ FUZZY LOGIC] ---\n");
    Serial.printf("Input - So lan sai: %d | Khoang cach thoi gian: %lu ms\n", wrongs, timeDiff);
    Serial.printf("Membership - Do mờ Nhanh: %.2f | Do mờ Chậm: %.2f\n", mu_Nhanh, mu_Cham);
    Serial.printf("=> Thoi gian phat tinh toan theo Logic Mo: %lu ms\n", penaltyTime);
    Serial.println("-----------------------------");

    return penaltyTime;
}