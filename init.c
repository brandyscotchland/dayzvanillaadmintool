void main()
{
    // WETTER INITIIEREN BEVOR DIE ÖKONOMIE INITIIERT WIRD
    Weather weather = g_Game.GetWeather();

    weather.MissionWeather(false); // false = Wettersteuerung aus Weather.c verwenden

    weather.GetOvercast().Set(Math.RandomFloatInclusive(0.4, 0.6), 1, 0);
    weather.GetRain().Set(0, 0, 1);
    weather.GetFog().Set(Math.RandomFloatInclusive(0.05, 0.1), 1, 0);

    // ÖKONOMIE INITIIEREN
    Hive hive = CreateHive();
    if (hive)
        hive.InitOffline();

    // DATUM NACH ÖKONOMIE-INIT RESETTEN
    int year, month, day, hour, minute;
    int resetMonth = 9, resetDay = 20;
    GetGame().GetWorld().GetDate(year, month, day, hour, minute);

    if ((month == resetMonth) && (day < resetDay))
    {
        GetGame().GetWorld().SetDate(year, resetMonth, resetDay, hour, minute);
    }
    else
    {
        if ((month == resetMonth + 1) && (day > resetDay))
        {
            GetGame().GetWorld().SetDate(year, resetMonth, resetDay, hour, minute);
        }
        else
        {
            if ((month < resetMonth) || (month > resetMonth + 1))
            {
                GetGame().GetWorld().SetDate(year, resetMonth, resetDay, hour, minute);
            }
        }
    }
}

class CustomMission: MissionServer
{
    // SteamIDs aller Admin-Spieler werden hier gespeichert
    private ref TStringArray adminList;

    override void OnInit()
    {
        super.OnInit();

        adminList = new TStringArray;
        LoadAdminList();
    }

    void LoadAdminList()
    {
        string path = "$profile:admins.txt";

        FileHandle file = OpenFile(path, FileMode.READ);

        // Wenn die Datei nicht existiert, erstelle sie
        if (file == 0) {
            file = OpenFile(path, FileMode.WRITE);

            FPrintln(file, "// Diese Datei enthält die SteamID64 aller Server-Admins. Füge sie unten hinzu.");
            FPrintln(file, "// Zeilen, die mit // beginnen, sind Kommentarzeilen.");

            CloseFile(file);
            return;
        }

        string line;

        while (FGets(file, line) > 0)
        {
            if (line.Length() < 2) continue;
            if (line.Get(0) + line.Get(1) == "//") continue;

            adminList.Insert(line);
        }

        CloseFile(file);
    }

    bool ExecuteCommand(PlayerBase player, string command)
    {
        // Teile den Befehl in Argumente auf
        TStringArray args = new TStringArray;
        SplitText(command, " ", args);

        switch (args[0])
        {
            case "/spawn":
                if (args.Count() != 2) {
                    SendPlayerMessage(player, "Syntax: /spawn [ITEM_NAME] - Spawnt einen Gegenstand.");
                    return false;
                }
                if (SpawnItem(player, args[1])) {
                    SendPlayerMessage(player, args[1] + " wurde gespawnt.");
                } else {
                    SendPlayerMessage(player, "FEHLER: Gegenstand konnte nicht gespawnt werden.");
                }
                break;

            case "/heal":
                HealPlayer(player);
                SendPlayerMessage(player, "Du wurdest geheilt.");
                break;

            case "/teleport":
                if (args.Count() != 3) {
                    SendPlayerMessage(player, "Syntax: /teleport [X] [Z] - Teleportiert zum angegebenen Punkt.");
                    return false;
                }
                string position = args[1] + " 0 " + args[2];
                TeleportPlayer(player, position);
                SendPlayerMessage(player, "Du wurdest zu den Koordinaten teleportiert: " + position);
                break;

            case "/vehicle":
                if (args.Count() != 2) {
                    SendPlayerMessage(player, "Verfügbare Fahrzeuge: olga, offroad, m3s, m3s_blue, m3s_orange, sarka, gunter, m1025");
                    return false;
                }
                if (SpawnVehicle(player, args[1])) {
                    SendPlayerMessage(player, args[1] + " wurde gespawnt.");
                } else {
                    SendPlayerMessage(player, "FEHLER: Fahrzeug konnte nicht gespawnt werden.");
                }
                break;

            default:
                SendPlayerMessage(player, "Unbekannter Befehl!");
                break;
        }

        return true;
    }

    bool SpawnItem(PlayerBase player, string itemName)
    {
        vector pos = player.GetPosition();
        pos[0] = pos[0] + 1;
        pos[1] = pos[1] + 1;
        pos[2] = pos[2] + 1;

        EntityAI item = player.SpawnEntityOnGroundPos(itemName, pos);

        if (item) {
            return true;
        }

        return false;
    }

	bool SpawnVehicle(PlayerBase player, string vehicleName)
{
    vector pos = player.GetPosition();
    pos[0] = pos[0] + 3; // Fahrzeug wird 3 Meter vom Spieler entfernt gespawnt
    pos[1] = pos[1] + 1;
    pos[2] = pos[2] + 3;

    Car vehicle;
    vehicleName.ToLower(); // Konvertiere den Fahrzeugnamen in Kleinbuchstaben

    // Erstellen und Konfigurieren des Fahrzeugs basierend auf dem Namen
    if (vehicleName == "olga")
    {
        vehicle = Car.Cast(GetGame().CreateObject("CivilianSedan", pos));
        vehicle.GetInventory().CreateAttachment("CivSedanHood");
        vehicle.GetInventory().CreateAttachment("CivSedanTrunk");
        vehicle.GetInventory().CreateAttachment("CivSedanDoors_Driver");
        vehicle.GetInventory().CreateAttachment("CivSedanDoors_CoDriver");
        vehicle.GetInventory().CreateAttachment("CivSedanDoors_BackLeft");
        vehicle.GetInventory().CreateAttachment("CivSedanDoors_BackRight");
        vehicle.GetInventory().CreateAttachment("CivSedanWheel");
        vehicle.GetInventory().CreateAttachment("CivSedanWheel");
        vehicle.GetInventory().CreateAttachment("CivSedanWheel");
        vehicle.GetInventory().CreateAttachment("CivSedanWheel");
    }
    else if (vehicleName == "offroad")
    {
        vehicle = Car.Cast(GetGame().CreateObject("OffroadHatchback", pos));
        vehicle.GetInventory().CreateAttachment("HatchbackTrunk");
        vehicle.GetInventory().CreateAttachment("HatchbackHood");
        vehicle.GetInventory().CreateAttachment("HatchbackDoors_CoDriver");
        vehicle.GetInventory().CreateAttachment("HatchbackDoors_Driver");
        vehicle.GetInventory().CreateAttachment("HatchbackWheel");
        vehicle.GetInventory().CreateAttachment("HatchbackWheel");
        vehicle.GetInventory().CreateAttachment("HatchbackWheel");
        vehicle.GetInventory().CreateAttachment("HatchbackWheel");
    }
    else if (vehicleName == "m1025")
    {
        vehicle = Car.Cast(GetGame().CreateObject("Offroad_02", pos));
        vehicle.GetInventory().CreateAttachment("Offroad_02_Hood");
        vehicle.GetInventory().CreateAttachment("Offroad_02_Trunk");
        vehicle.GetInventory().CreateAttachment("Offroad_02_Door_1_1");
        vehicle.GetInventory().CreateAttachment("Offroad_02_Door_1_2");
        vehicle.GetInventory().CreateAttachment("Offroad_02_Door_2_1");
        vehicle.GetInventory().CreateAttachment("Offroad_02_Door_2_2");
        vehicle.GetInventory().CreateAttachment("Offroad_02_Wheel");
        vehicle.GetInventory().CreateAttachment("Offroad_02_Wheel");
        vehicle.GetInventory().CreateAttachment("Offroad_02_Wheel");
        vehicle.GetInventory().CreateAttachment("Offroad_02_Wheel");
		vehicle.GetInventory().CreateAttachment("Offroad_02_Wheel");
        vehicle.GetInventory().CreateAttachment("CarBattery");
        vehicle.GetInventory().CreateAttachment("GlowPlug");
    }
    else if (vehicleName == "sarka")
    {
        vehicle = Car.Cast(GetGame().CreateObject("Sedan_02", pos));
        vehicle.GetInventory().CreateAttachment("Sedan_02_Hood");
        vehicle.GetInventory().CreateAttachment("Sedan_02_Trunk");
        vehicle.GetInventory().CreateAttachment("Sedan_02_Door_1_1");
        vehicle.GetInventory().CreateAttachment("Sedan_02_Door_1_2");
        vehicle.GetInventory().CreateAttachment("Sedan_02_Door_2_1");
        vehicle.GetInventory().CreateAttachment("Sedan_02_Door_2_2");
        vehicle.GetInventory().CreateAttachment("Sedan_02_Wheel");
        vehicle.GetInventory().CreateAttachment("Sedan_02_Wheel");
        vehicle.GetInventory().CreateAttachment("Sedan_02_Wheel");
        vehicle.GetInventory().CreateAttachment("Sedan_02_Wheel");
    }
    else if (vehicleName == "gunter")
    {
        vehicle = Car.Cast(GetGame().CreateObject("Hatchback_02", pos));
        vehicle.GetInventory().CreateAttachment("Hatchback_02_Hood");
        vehicle.GetInventory().CreateAttachment("Hatchback_02_Trunk");
        vehicle.GetInventory().CreateAttachment("Hatchback_02_Door_1_1");
        vehicle.GetInventory().CreateAttachment("Hatchback_02_Door_1_2");
        vehicle.GetInventory().CreateAttachment("Hatchback_02_Door_2_1");
        vehicle.GetInventory().CreateAttachment("Hatchback_02_Door_2_2");
        vehicle.GetInventory().CreateAttachment("Hatchback_02_Wheel");
        vehicle.GetInventory().CreateAttachment("Hatchback_02_Wheel");
        vehicle.GetInventory().CreateAttachment("Hatchback_02_Wheel");
        vehicle.GetInventory().CreateAttachment("Hatchback_02_Wheel");
		vehicle.GetInventory().CreateAttachment("Hatchback_02_Wheel");
    }
    else if (vehicleName == "m3s")
    {
        vehicle = Car.Cast(GetGame().CreateObject("Truck_01_Covered", pos));
        vehicle.GetInventory().CreateAttachment("Truck_01_Hood");
        vehicle.GetInventory().CreateAttachment("Truck_01_WheelDouble");
		vehicle.GetInventory().CreateAttachment("Truck_01_WheelDouble");
		vehicle.GetInventory().CreateAttachment("Truck_01_WheelDouble");
		vehicle.GetInventory().CreateAttachment("Truck_01_WheelDouble");
		vehicle.GetInventory().CreateAttachment("Truck_01_Wheel");
        vehicle.GetInventory().CreateAttachment("Truck_01_Wheel");
        vehicle.GetInventory().CreateAttachment("Truck_01_Wheel");
        vehicle.GetInventory().CreateAttachment("Truck_01_Wheel");
        vehicle.GetInventory().CreateAttachment("Truck_01_Door_1_1");
        vehicle.GetInventory().CreateAttachment("Truck_01_Door_2_1");
        vehicle.GetInventory().CreateAttachment("TruckBattery");
    }
    else if (vehicleName == "m3s_blue")
    {
        vehicle = Car.Cast(GetGame().CreateObject("Truck_01_Covered_Blue", pos));
        vehicle.GetInventory().CreateAttachment("Truck_01_Hood");
        vehicle.GetInventory().CreateAttachment("Truck_01_WheelDouble");
        vehicle.GetInventory().CreateAttachment("Truck_01_WheelDouble");
		vehicle.GetInventory().CreateAttachment("Truck_01_WheelDouble");
		vehicle.GetInventory().CreateAttachment("Truck_01_WheelDouble");
		vehicle.GetInventory().CreateAttachment("Truck_01_Wheel");
		vehicle.GetInventory().CreateAttachment("Truck_01_Wheel");
        vehicle.GetInventory().CreateAttachment("Truck_01_Wheel");
        vehicle.GetInventory().CreateAttachment("Truck_01_Wheel");
        vehicle.GetInventory().CreateAttachment("Truck_01_Door_1_1");
        vehicle.GetInventory().CreateAttachment("Truck_01_Door_2_1");
        vehicle.GetInventory().CreateAttachment("TruckBattery");
    }
    else if (vehicleName == "m3s_orange")
    {
        vehicle = Car.Cast(GetGame().CreateObject("Truck_01_Covered_Orange", pos));
        vehicle.GetInventory().CreateAttachment("Truck_01_Hood");
        vehicle.GetInventory().CreateAttachment("Truck_01_WheelDouble");
		vehicle.GetInventory().CreateAttachment("Truck_01_WheelDouble");
		vehicle.GetInventory().CreateAttachment("Truck_01_WheelDouble");
		vehicle.GetInventory().CreateAttachment("Truck_01_WheelDouble");
        vehicle.GetInventory().CreateAttachment("Truck_01_Wheel");
        vehicle.GetInventory().CreateAttachment("Truck_01_Wheel");
        vehicle.GetInventory().CreateAttachment("Truck_01_Wheel");
		vehicle.GetInventory().CreateAttachment("Truck_01_Wheel");
        vehicle.GetInventory().CreateAttachment("Truck_01_Door_1_1");
        vehicle.GetInventory().CreateAttachment("Truck_01_Door_2_1");
        vehicle.GetInventory().CreateAttachment("TruckBattery");
    }
    else
    {
        SendPlayerMessage(player, "FEHLER: Unbekannter Fahrzeugname.");
        return false;
    }

    if (vehicle)
    {
        // Generelle Konfiguration für alle Fahrzeuge
        vehicle.GetInventory().CreateAttachment("CarRadiator");
        vehicle.GetInventory().CreateAttachment("CarBattery");
        vehicle.GetInventory().CreateAttachment("SparkPlug");
        vehicle.GetInventory().CreateAttachment("HeadlightH7");
        vehicle.GetInventory().CreateAttachment("HeadlightH7");

        // Flüssigkeiten auffüllen
        vehicle.Fill(CarFluid.FUEL, vehicle.GetFluidCapacity(CarFluid.FUEL));
        vehicle.Fill(CarFluid.OIL, vehicle.GetFluidCapacity(CarFluid.OIL));
        vehicle.Fill(CarFluid.BRAKE, vehicle.GetFluidCapacity(CarFluid.BRAKE));
        vehicle.Fill(CarFluid.COOLANT, vehicle.GetFluidCapacity(CarFluid.COOLANT));

        // Fahrzeug in den Leerlauf schalten
        vehicle.GetController().ShiftTo(CarGear.NEUTRAL);

        return true;
    }

    return false;
}





    void HealPlayer(PlayerBase player)
    {
        // Setze die Gesundheit auf das Maximum
        player.SetHealth("GlobalHealth", "Blood", player.GetMaxHealth("GlobalHealth", "Blood"));
        player.SetHealth("GlobalHealth", "Health", player.GetMaxHealth("GlobalHealth", "Health"));
        player.SetHealth("GlobalHealth", "Shock", player.GetMaxHealth("GlobalHealth", "Shock"));

        // Setze den Hunger auf das Maximum
        player.GetStatEnergy().Set(player.GetStatEnergy().GetMax());

        // Setze den Durst auf das Maximum
        player.GetStatWater().Set(player.GetStatWater().GetMax());
    }

    void TeleportPlayer(PlayerBase player, string position)
    {
        vector pos = position.ToVector();
        pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
        player.SetPosition(pos);
    }

    override void OnEvent(EventType eventTypeId, Param params)
    {
        switch (eventTypeId)
        {
            case ChatMessageEventTypeID:

                ChatMessageEventParams chatParams;
                Class.CastTo(chatParams, params);

                string command = string.ToString(chatParams.param3, false, false, false);

                if (command.Get(0) != "/") break;

                string senderName = string.ToString(chatParams.param2, false, false, false);
                PlayerBase sender = GetPlayerByName(senderName, IdentityType.NAME);

                if (!sender) {
                    return;
                }

                if (!IsAdmin(sender)) {
                    SendPlayerMessage(sender, "Entschuldigung, du bist kein Admin!");
                    return;
                }

                ExecuteCommand(sender, command);
                return;
        }

        super.OnEvent(eventTypeId, params);
    }

    bool IsAdmin(PlayerBase player)
    {
        return adminList.Find(player.GetIdentity().GetPlainId()) != -1;
    }

    PlayerBase GetPlayerByName(string name, IdentityType type)
    {
        ref array<Man> playerList = new array<Man>;
        GetGame().GetPlayers(playerList);

        PlayerBase player;

        for (int i = 0; i < playerList.Count(); ++i)
        {
            Class.CastTo(player, playerList.Get(i));

            if (type == IdentityType.ANY) {
                if (player.GetIdentity().GetName() == name || player.GetIdentity().GetPlainId() == name || player.GetID() == name.ToInt())
                    return player;
            }
            else if (type == IdentityType.NAME && player.GetIdentity().GetName() == name) {
                return player;
            }
            else if (type == IdentityType.STEAMID && player.GetIdentity().GetPlainId() == name) {
                return player;
            }
            else if (type == IdentityType.PID && player.GetID() == name.ToInt()) {
                return player;
            }
        }

        return NULL;
    }

    void SendPlayerMessage(PlayerBase player, string message)
    {
        Param1<string> msgParam;
        msgParam = new Param1<string>(message);
        GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, msgParam, true, player.GetIdentity());
    }

    void SplitText(string text, string delimiter, out TStringArray list)
    {
        string temp = text + delimiter;
        string word = "";

        for (int i = 0; i < temp.Length(); i++)
        {
            string x = temp.Get(i);

            if (x != delimiter) {
                word = word + x;
            } else {
                list.Insert(word);
                word = "";
            }
        }
    }

    override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
    {
        Entity playerEnt;
        playerEnt = GetGame().CreatePlayer(identity, characterName, pos, 0, "NONE");
        Class.CastTo(m_player, playerEnt);

        GetGame().SelectPlayer(identity, m_player);

        return m_player;
    }

    override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
    {
        EntityAI top;
        EntityAI itemEnt;
        ItemBase itemBs;

        top = player.FindAttachmentBySlotName("Body");

        if (top)
        {
            itemEnt = top.GetInventory().CreateInInventory("Rag");

            if (Class.CastTo(itemBs, itemEnt))
                itemBs.SetQuantity(6);

            itemEnt = player.GetInventory().CreateInInventory("TunaCan");
            itemEnt = top.GetInventory().CreateInInventory("VitaminBottle");
            itemEnt = top.GetInventory().CreateInInventory("TetracyclineAntibiotics");
        }
    }
};

enum IdentityType {
    ANY,
    NAME,
    STEAMID,
    PID
};

Mission CreateCustomMission(string path)
{
    return new CustomMission();
}
