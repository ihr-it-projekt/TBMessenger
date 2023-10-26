class TBMOnlinePlayer
{
    string id;
    vector position;
	string name;

    void TBMOnlinePlayer(string _id, string _name, vector _position = "0 0 0") {
        this.id = _id;
        this.name = _name;
        this.position = _position;
    }
}
