#include "util.hpp"

#include "creation_callbacks.hpp"
#include "db.hpp"

using namespace std;

class SimpleCols: public Gtk::ListStore::ColumnRecord {
 public:
  Gtk::TreeModelColumn<std::string> colName;
  Gtk::TreeModelColumn<int>         colID;

  SimpleCols() {
    add(colName);
    add(colID);
  }
};


void create_materials(Gtk::Builder* builder) {
  Gtk::Button *  addMisc, *delMisc, *addType, *delType;
  Gtk::Entry *   compName, *compPrice, *typeName, *typeDesc, *typePrice, *avgConcrete, *avgNail;
  Gtk::TreeView *compList, *typeList;

  builder->get_widget("addComponents", addMisc);  // misc components
  builder->get_widget("delComponents", delMisc);
  builder->get_widget("addWoodTypes", addType);
  builder->get_widget("delWoodTypes", delType);

  builder->get_widget("nameInputComp", compName);
  builder->get_widget("pricePerUnitInput", compPrice);
  builder->get_widget("nameInputType", typeName);
  builder->get_widget("descInputType", typeDesc);
  builder->get_widget("priceInputType", typePrice);
  builder->get_widget("avgPriceInputConcrete", avgConcrete);
  builder->get_widget("avgPriceInputNail", avgNail);

  builder->get_widget("componentList", compList);
  builder->get_widget("woodTypeList", typeList);

  static SimpleCols compCols, typeCols;
  auto              compModel = Gtk::ListStore::create(compCols);
  auto              typeModel = Gtk::ListStore::create(typeCols);


  auto get_selected_comp = [=]() {
    std::optional<Component> res;
    auto                     sel = compList->get_selection()->get_selected();
    if (sel) { res = DB::get_component(sel->get_value(compCols.colID)); }
    return res;
  };
  auto get_selected_type = [=]() {
    std::optional<WoodType> res;
    auto                    sel = typeList->get_selection()->get_selected();
    if (sel) { res = DB::get_woodtype(sel->get_value(typeCols.colID)); }
    return res;
  };
  auto reset_complist = [=]() {
    auto comps = DB::get_comps_of_type(ComponentType::Misc);
    compModel->clear();
    for (auto comp : comps) {
      auto row              = *compModel->append();
      row[compCols.colName] = comp.name;
      row[compCols.colID]   = comp.id;
    }
  };
  auto reset_typelist = [=]() {
    auto types = DB::get_woodtypes();
    typeModel->clear();
    for (auto type : types) {
      auto row              = *typeModel->append();
      row[typeCols.colName] = type.name;
      row[typeCols.colID]   = type.id;
    }
  };

  compList->set_model(compModel);
  typeList->set_model(typeModel);

  reset_complist();
  reset_typelist();

  addMisc->signal_clicked().connect([=]() {
    auto newCom           = DB::new_component();
    auto row              = *compModel->append();
    row[compCols.colName] = newCom.name;
    row[compCols.colID]   = newCom.id;
  });
  delMisc->signal_clicked().connect([=]() {
    auto selected = get_selected_comp();
    if (!selected.has_value()) return;
    selected->remove();
    reset_complist();
  });
  addType->signal_clicked().connect([=]() {
    auto newType          = DB::new_woodtype();
    auto row              = *typeModel->append();
    row[typeCols.colName] = newType.name;
    row[typeCols.colID]   = newType.id;
  });
  delType->signal_clicked().connect([=]() {
    auto selected = get_selected_type();
    if (!selected.has_value()) return;
    selected->remove();
    reset_typelist();
  });

  // the static selections are my attempt at a minor optimization to reduce the number of
  // DB accesses
  static std::optional<Component> selComp;
  compList->get_selection()->signal_changed().connect([=]() {
    selComp = get_selected_comp();
    if (!selComp.has_value()) return;

    cout << "Selected component " << selComp->name << endl;

    compName->set_text(selComp->name);
    compPrice->set_text(double_to_string(selComp->pricePerUnit));
  });
  static std::optional<WoodType> selType;
  typeList->get_selection()->signal_changed().connect([=]() {
    selType = get_selected_type();
    if (!selType.has_value()) return;

    cout << "Selected WoodType " << selType->name << endl;

    typeName->set_text(selType->name);
    typePrice->set_text(double_to_string(selType->pricePerBF));
    typeDesc->set_text(selType->desc);
  });

  compName->signal_changed().connect([=]() {
    if (!selComp.has_value()) return;
    selComp->name = compName->get_text();
    selComp->update();
  });
  compPrice->signal_changed().connect([=]() {
    if (!selComp.has_value()) return;
    auto val = parse_double(compPrice->get_text());
    if (!val.has_value()) return;
    selComp->pricePerUnit = *val;
    selComp->update();
  });

  typeName->signal_changed().connect([=]() {
    if (!selType.has_value()) return;
    selType->name = typeName->get_text();
    selType->update();
  });
  typeDesc->signal_changed().connect([=]() {
    if (!selType.has_value()) return;
    selType->desc = typeDesc->get_text();
    selType->update();
  });
  typePrice->signal_changed().connect([=]() {
    if (!selType.has_value()) return;
    auto val = parse_double(typePrice->get_text());
    if (!val.has_value()) return;
    selType->pricePerBF = *val;
    selType->update();
  });

  avgConcrete->signal_changed().connect([=]() {
    auto val = parse_double(avgConcrete->get_text());
    if (!val.has_value()) return;
    cout << "Parsed " << *val << endl;
    DB::set_avgprice(ComponentType::Concrete, *val);
  });
  avgConcrete->set_text(double_to_string(DB::get_avgprice(ComponentType::Concrete)));

  avgNail->signal_changed().connect([=]() {
    auto val = parse_double(avgNail->get_text());
    if (!val.has_value()) return;
    cout << "Parsed " << *val << endl;
    DB::set_avgprice(ComponentType::Nail, *val);
  });
  avgNail->set_text(double_to_string(DB::get_avgprice(ComponentType::Nail)));
}
