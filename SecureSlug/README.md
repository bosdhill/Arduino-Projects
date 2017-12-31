# Project: Secure Slug

Some background:
The Secure Slug was a robotics project I collaborated on with Ian Van Steen, Clare Rickard and Vidisha Nevatia for the 
Thales Arduino Competition. We created an autonomous secure goods transferring vehicle. It employed various sensors such 
as photo resistors for autonmous guidance, buttons for password entry, an accelerometer, and a piezo element. The proof 
of concept is that an autonomous bot could successfully be used as a secure peer to peer goods transfer for businesses, 
where the customers can deposit goods and the business can withdraw them. It can also be modified for distribution, i.e. 
the business could deposit some goods to various customers. For example the business can be a bank, and the customers 
can be bank patrons who are depositing money. 

# File: Gate.ino

Description: Gate.ino operates the gate, which is the how the user interacts with the robot. It allows the user to create 
a withdraw password and a deposit password, which are used for other users to either open the gate for withdrawal or
for deposit. If the incorrect password is entered, the gate will not open and instead a red light will flash. 
