from create import buildDatabase

dataBase = buildDatabase()
dataBase.add_account("Nguyen Minh Phat", "st.tru", "0911311391")
dataBase.add_account("Phan Chi Cong", "st.reu", "0967999302")
dataBase.add_account("Tan Van Thai", "st.lol", "0903782999")
dataBase.add_account("Nguyen Bao", "st.cmt8", "0797282945")
dataBase.rem_account("Nguyen Thanh Son")
dataBase.rem_account("Nguyen Thanh Hoa")
dataBase.rem_account("Tran Van A")
