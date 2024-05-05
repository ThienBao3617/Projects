function showSection(section) {
  // Ẩn tất cả các phần
  $(".admin-section").hide();
  // Hiển thị phần được chọn
  $("#" + section).show();
}

function logout() {
  window.location.href = "/";
}
function updateCost() {
  // Chuyển đổi giá trị nhập vào thành số và lưu vào Local Storage
  var value = document.getElementById("priceInput").value;
  localStorage.setItem("cost", parseFloat(value));
  console.log("Giá tiền đã được cập nhật: " + value);
}
